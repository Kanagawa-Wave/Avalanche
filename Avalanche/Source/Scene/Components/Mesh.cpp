#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderer/Context.h"

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
{
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

namespace std
{
	template <>
	struct hash<ModelVertex>
	{
		size_t operator()(ModelVertex const& vertex) const noexcept
		{
			size_t seed = 0;
			hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

Mesh::Mesh(const std::string& meshPath)
	: m_MeshPath(meshPath)
{
	LoadMeshFromFile(meshPath);

	m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(ModelVertex));
	m_VertexBuffer->SetLayout(ModelVertex::Layout());

	m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
}

Mesh::Mesh(const aiMesh* mesh)
{
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex;
		vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		if (mesh->mTextureCoords[0])
			vertex.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		m_Vertices.push_back(vertex);
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			m_Indices.push_back(face.mIndices[j]);
		}
	}

	m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(ModelVertex));
	m_VertexBuffer->SetLayout(ModelVertex::Layout());

	m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
}

void Mesh::Bind(vk::CommandBuffer commandBuffer, const vk::PipelineLayout& layout) const
{
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 1, {m_DescriptorSet->GetDescriptorSet()}, nullptr);
	m_VertexBuffer->Bind(commandBuffer);
	m_IndexBuffer->Bind(commandBuffer);
}

void Mesh::Draw(vk::CommandBuffer commandBuffer) const
{
	commandBuffer.drawIndexed(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
}

void Mesh::SetTexture(const std::string& path)
{
	m_Texture = std::make_unique<Texture>(path, ETextureFormat::Linear);
	m_TexturePath = path;

	if (!m_DescriptorSet)
	{
		m_DescriptorSet = Context::Instance().GetDescriptorSetBuilder()->CreateDescriptorSet(EDescriptorSetLayoutType::PerModelSet);
	}
	m_DescriptorSet->UpdateDescriptor(m_Texture.get(), 0);
}

void Mesh::LoadObjFromFile(const std::string& path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;

	if (!tinyobj::LoadObj(
		&attrib,
		&shapes,
		&materials,
		&warn,
		&error,
		path.c_str()))
	{
		LOG_E("{0}", warn + error)
	}

	std::unordered_map<ModelVertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			ModelVertex vertex{};
			if (index.vertex_index >= 0)
			{
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};
				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
				};
			}
			if (index.normal_index >= 0)
			{
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};
			}
			if (index.texcoord_index >= 0)
			{
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1 - attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}


			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}
			m_Indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void Mesh::LoadMeshFromFile(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);

	uint32_t vertexOffset = 0;

	for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		
		// Copy vertices
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
			ModelVertex vertex{};
			vertex.position = { mesh->mVertices[v].x,
								  mesh->mVertices[v].y,
								  mesh->mVertices[v].z };
			if (mesh->HasNormals())
			{
				vertex.normal = { mesh->mNormals[v].x,
									mesh->mNormals[v].y,
									mesh->mNormals[v].z };
			}
			if (mesh->mTextureCoords[0])
			{
				vertex.uv = { mesh->mTextureCoords[0][v].x,
								mesh->mTextureCoords[0][v].y };
			}
			m_Vertices.push_back(vertex);
		}
		
		// Copy faces (indices), with offset
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			const aiFace& face = mesh->mFaces[f];
			for (unsigned int i = 0; i < face.mNumIndices; ++i) {
				m_Indices.push_back(face.mIndices[i] + vertexOffset);
			}
		}

		vertexOffset += mesh->mNumVertices;
	}
}
