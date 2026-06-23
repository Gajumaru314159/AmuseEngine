//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Feature/MeshRenderFeature.h>
#include "Amuse/Core/Geometry/Bounding.h"

namespace Amuse::RPI {

	void MeshRenderFeature::update(UpdateContext& context)
	{
		processCommands(context);
		buildPackets(context);
	}

	void MeshRenderFeature::processCommands(UpdateContext& context) {

		// オブジェクト追加をm_proxies等にマージ
		for(auto& command:m_commands) {
			if(command.type == MeshCommandType::Add) {
                // パック配列の後ろに要素を追加
                {
                    command.proxy->gindex = m_proxies.size();
                    m_proxies.emplace_back(command.proxy);
                    m_matrices.emplace_back(command.matrix);
                    m_bounds.emplace_back(command.bounds);

                    AMUSE_ASSERT_EXPR(m_proxies.size()==m_matrices.size());
                }

				// UnitSetに追加
                {
                    // MeshとMaterial[]の組み合わせ
                    // VertexとIndexのBufferごとに分ける必要がある
                    // MeshPoolを実装すればMeshごとの分岐は不要になる？
                    // MeshPoolにする場合は複数のBufferに分けるとIndirect描画が一部のプラットフォームでできなくなるので注意
                    const MeshUnitKey& key = command.proxy->key;

                    auto& unit = m_units[key];

                    command.proxy->uindex = unit.proxies.size();
                    unit.proxies.emplace_back(command.proxy);
                }
			}
			if(command.type == MeshCommandType::Remove) {
                // Unit削除
                {
                    const MeshUnitKey& key = command.proxy->key;
                    auto& unit = m_units[key];

                    auto i = command.proxy->uindex;
                    auto j = unit.proxies.size();

                    unit.proxies.at(j)->gindex = unit.proxies.at(i)->gindex;
                    unit.proxies.at(j)->uindex = unit.proxies.at(i)->uindex;
                    std::swap(unit.proxies.at(i),unit.proxies.at(j));
                    unit.proxies.pop_back();

                    // TODO Unitが空になったら削除(Throttle処理入れたほうがいいかも)
                }
                // Global削除
                // やっていることはECSなのだから自前でスワップを書いているのもばからしい
                // どうせColorとかインスタンスごとに持たせる情報は増える
                {
                    auto i = command.proxy->gindex;
                    auto j = m_proxies.size();
                    m_proxies.at(j)->gindex = m_proxies.at(i)->gindex;
                    m_matrices.at(j) = m_matrices.at(i);
                    m_bounds.at(j) = m_bounds.at(i);
                    std::swap(m_proxies.at(i),m_proxies.at(j));
                    std::swap(m_matrices.at(i),m_matrices.at(j));
                    std::swap(m_bounds.at(i),m_bounds.at(j));
                    m_proxies.pop_back();
                    m_matrices.pop_back();
                    m_bounds.pop_back();
                }
			}
			if(command.type == MeshCommandType::Update) {
				m_matrices.at(command.proxy->gindex) = command.matrix;
				m_bounds.at(command.proxy->gindex) = command.bounds;
			}
		}

    }

    s32 CalculateLOD(RenderView* view, const Bounds& bounds) {
        // TODO LOD計算
        return 0;
    }
    bool CalculateCulling(RenderView* view, const Bounds& bounds)
	{
	    return false;
	}

	void MeshRenderFeature::buildPackets(UpdateContext& context)
	{
        // auto instances = m_instances.alloc(12);
        // instances[0];

        for(auto view:context.views) {
            if(auto materialRF = view->findFeature<MaterialRenderFeature>()) {

                auto& mvd = view->get<MeshViewData>();

                // LOD計算
                mvd.cpu.lods.clear();
                for(auto& bounds:m_bounds) {
                    mvd.cpu.lods.emplace_back(CalculateLOD(view, bounds));
                }

                // カリング
                mvd.cpu.culls.clear();
                for(auto& bounds:m_bounds) {
                    mvd.cpu.culls.emplace_back(CalculateCulling(view, bounds));
                }

                // 描画アイテムの構築
                mvd.cpu.units.clear();
                for(auto& [ukey,unit] : m_units) {
                    auto& vunit = mvd.cpu.units[ukey];

                    // インスタンスごとにLODとカリングの結果を見てLODごとのインスタンス列に振り分ける
                    for(auto proxy:unit.proxies) {
                        auto instance = proxy->gindex;
                        if(!mvd.cpu.culls[instance]) {
                            s32 level = mvd.cpu.lods[instance];
                            vunit.lods[level].instances.push_back(instance);
                        }
                    }
                }

                // インスタンス列を結合
                mvd.cpu.instances.clear();
                for(auto& [ukey,vunit] : mvd.cpu.units) {
                    for(auto& vlod : vunit.lods) {
                        vlod.instanceStart = static_cast<u32>(mvd.cpu.instances.size());
                        vlod.instanceCount = static_cast<u32>(vlod.instances.size());
                        for(s32 instance:vlod.instances) {
                            mvd.cpu.instances.push_back(instance);
                        }
                    }
                }

                // 描画引数を生成
                mvd.cpu.packets.clear();
                for(auto& [ukey,unit] : m_units) {
                    auto& vunit = mvd.cpu.units[ukey];

                    for(auto [level,lod] : Indexed(ukey.mesh->lods)) {
                        auto& vlod = vunit.lods[level];

                        for(auto& section:lod.sections) {

                            auto& resource = ukey.mesh->lods[level];

                            MaterialDrawPacket packet {
                                .material = ukey.overrideMaterials.empty() ? ukey.mesh->materials.at(section.material) : ukey.overrideMaterials.at(section.material),
                                .layout = resource.buffer.layout,
                                .vertices = resource.buffer.vertices,
                                .indices = resource.buffer.indices,
                                .args = {
                                    .startVertex = (u32)section.baseVertex,
                                    .startIndex = (u32)section.indexStart,
                                    .indexCount = (u32)section.indexCount,
                                    .startInstance = (u32)vlod.instanceStart,
                                    .instanceCount = (u32)vlod.instanceCount,
                                },
                            };
                            mvd.cpu.packets.push_back(packet);
                        }
                    }
                }

                // GPUにデータを転送
                {
                    auto& block = view->get<MaterialViewData>().block;

                    mvd.gpu.update(mvd.cpu);
                    block.setBuffer("Instances", mvd.gpu.instances);
                }

                // MaterialRenderFeatureに登録
                materialRF->addDrawPackets(view,mvd.cpu.packets);
            }
        }

        // マトリックスを転送(サイズが足りない場合は再生成)
        // m_matrixBuffer->update();
        context.scene->findFeature<MaterialRenderFeature>()->getSceneBlock().setBuffer("Matrices",m_matrixBuffer);
	}


    void MeshRenderFeature::MeshViewData::GPUData::update(const CPUData& cpu)
	{
	    // バッファが未生成orサイズが足りない場合は再生成
	    // cpuのデータをgpuに書き込む
    }

}
