#include "pch.h"
#include "MaterialRepository.h"
#include "TextureRepository.h"

MaterialRepository::~MaterialRepository() = default;
MaterialRepository::MaterialRepository(TextureRepository* texRepository)
    : m_texRepository(texRepository)
{}

MaterialHandle MaterialRepository::Create(TextureHandle th)
{
    auto texEntry = m_texRepository->Get(th);
    if (!texEntry) return MaterialHandle::Invalid();
    assert(texEntry->texRes != nullptr); //텍스쳐가 어떤 state에서도 빈껍데기texRes라도 존재해야 함.

    MaterialEntry entry;
    entry.texRes = texEntry->texRes.get();
    entry.sourceTextureHandle = th;
    auto materialState = (texEntry->state == TextureState::Ready) ? MaterialState::Ready : MaterialState::Pending;
    entry.state = materialState;

    MaterialHandle h = m_pool.Emplace(move(entry));
    if (materialState == MaterialState::Pending)
        m_loadingList.push_back(h);

    return h;
}

void MaterialRepository::Update()
{
    size_t write = 0;

    for (size_t read = 0; read < m_loadingList.size(); ++read)
    {
        MaterialHandle mh = m_loadingList[read];
        auto m = m_pool.Find(mh);
        if (!m) continue;

        auto texEntry = m_texRepository->Get(m->sourceTextureHandle);
        if (!texEntry)
        {
            m->state = MaterialState::Failed;
            continue;
        }

        if (texEntry->state == TextureState::Ready) 
            m->state = MaterialState::Ready;
        else if (texEntry->state == TextureState::Failed) 
            m->state = MaterialState::Failed;
        else 
            m_loadingList[write++] = mh;
    }

    m_loadingList.resize(write);
}