#include "terrain.h"

void Terrain::BuildTerrain(unsigned seed)
{
    // Generate heights
    heights.assign(gridW * gridH, 0.0f);
    Perlin pn(seed);
    float minH = 1e9f, maxH = -1e9f;
    for (int y = 0; y < gridH; y++)
        for (int x = 0; x < gridW; x++)
        {
            float h = heightSample(pn, (float)x, (float)y);
            heights[y * gridW + x] = h;
            minH = std::min(minH, h);
            maxH = std::max(maxH, h);
        }
    // Normalize to [0,maxHeight]
    for (float &h : heights)
    {
        h = (h - minH) / (maxH - minH + 1e-6f) * maxHeight;
    }

    // Build blocked map by slope and waterline
    blocked.assign(gridW * gridH, 0);
    for (int y = 1; y < gridH - 1; y++)
    {
        for (int x = 1; x < gridW - 1; x++)
        {
            float h = heights[y * gridW + x];
            float hx1 = heights[y * gridW + (x - 1)];
            float hx2 = heights[y * gridW + (x + 1)];
            float hy1 = heights[(y - 1) * gridW + x];
            float hy2 = heights[(y + 1) * gridW + x];
            Vector3 dx = {cellSize, hx2 - hx1, 0};
            Vector3 dz = {0, hy2 - hy1, cellSize};
            Vector3 n = Vector3Normalize(Vector3CrossProduct(dz, dx));
            float cosUp = Vector3DotProduct(n, {0, 1, 0});
            bool steep = cosUp < steepLimit; // small dot => steep
            bool water = h < 0.5f;             // optional waterline
            blocked[y * gridW + x] = (steep || water) ? 1 : 0;
        }
    }

    // Build a simple grid mesh (triangle grid) based on heights
    // Vertices: gridW*gridH, Indices: (gridW-1)*(gridH-1)*6
    int vcount = gridW * gridH;
    int tcount = (gridW - 1) * (gridH - 1) * 2;

    std::vector<Vector3> verts(vcount);
    std::vector<Vector3> norms(vcount, {0, 1, 0});
    std::vector<Vector2> uvs(vcount);
    std::vector<unsigned short> indices(tcount * 3);

    for (int y = 0; y < gridH; y++)
    {
        for (int x = 0; x < gridW; x++)
        {
            int i = y * gridW + x;
            Vector3 p = cellToWorld(x, y);
            verts[i] = p;
            uvs[i] = {(float)x / (gridW - 1), (float)y / (gridH - 1)};
        }
    }
    // indices
    int k = 0;
    for (int y = 0; y < gridH - 1; y++)
    {
        for (int x = 0; x < gridW - 1; x++)
        {
            int i0 = y * gridW + x;
            int i1 = y * gridW + (x + 1);
            int i2 = (y + 1) * gridW + x;
            int i3 = (y + 1) * gridW + (x + 1);
            indices[k++] = (unsigned short)i0;
            indices[k++] = (unsigned short)i2;
            indices[k++] = (unsigned short)i1;
            indices[k++] = (unsigned short)i1;
            indices[k++] = (unsigned short)i2;
            indices[k++] = (unsigned short)i3;
        }
    }
    // compute normals via face accumulation
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        int a = indices[i], b = indices[i + 1], c = indices[i + 2];
        Vector3 e1 = Vector3Subtract(verts[b], verts[a]);
        Vector3 e2 = Vector3Subtract(verts[c], verts[a]);
        Vector3 n = Vector3CrossProduct(e1, e2);
        norms[a] = Vector3Add(norms[a], n);
        norms[b] = Vector3Add(norms[b], n);
        norms[c] = Vector3Add(norms[c], n);
    }
    for (auto &n : norms)
        n = Vector3Normalize(n);

    // Upload to raylib Mesh
    if (mesh.vboId != NULL)
        UnloadMesh(mesh);
    mesh = {0};
    mesh.vertexCount = vcount;
    mesh.triangleCount = tcount;
    mesh.vertices = (float *)MemAlloc(vcount * 3 * sizeof(float));
    mesh.normals = (float *)MemAlloc(vcount * 3 * sizeof(float));
    mesh.texcoords = (float *)MemAlloc(vcount * 2 * sizeof(float));
    mesh.indices = (unsigned short *)MemAlloc(indices.size() * sizeof(unsigned short));

    for (int i = 0; i < vcount; i++)
    {
        mesh.vertices[i * 3 + 0] = verts[i].x;
        mesh.vertices[i * 3 + 1] = verts[i].y;
        mesh.vertices[i * 3 + 2] = verts[i].z;
        mesh.normals[i * 3 + 0] = norms[i].x;
        mesh.normals[i * 3 + 1] = norms[i].y;
        mesh.normals[i * 3 + 2] = norms[i].z;
        mesh.texcoords[i * 2 + 0] = uvs[i].x;
        mesh.texcoords[i * 2 + 1] = uvs[i].y;
    }
    memcpy(mesh.indices, indices.data(), indices.size() * sizeof(unsigned short));

    UploadMesh(&mesh, false);

    if (model.meshCount > 0)
        UnloadModel(model);
    model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = Color{100, 180, 80, 255};
}

float Terrain::heightSample(Perlin &perlin, float x, float y)
{
    float f = 0.0f;
    float amp = 1.0f;
    float freq = 0.005f;
    for (int o = 0; o < 5; o++)
    {
        f += amp * perlin.noise3(x * freq, y * freq, 0.0f);
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return f; // [0, ~something]
}
    float Terrain::terrainHeightAt(float x, float z)
    {
        Vector2 c = worldToCell(x, z);
        int ix = (int)std::clamp((int)c.x, 0, gridW - 1);
        int iy = (int)std::clamp((int)c.y, 0, gridH - 1);
        return heights[iy * gridW + ix];
    }