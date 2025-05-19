/**
 * @file export_mesh_snapshot.ms
 * @brief Exports a snapshot of the first selected object as a static mesh header file for use in C/C++ applications.
 *
 * @details
 * Naming conventions:
 * - camelCase for variables and function-like constructs (e.g., exportMeshSnapshot, vertexCount)
 * - PascalCase for constants and exported arrays (e.g., TeaVertices, TeaFaceList)
 */

-- === Step 1: Get the mesh snapshot of the first selected object ===

/**
 * @brief Capture the snapshot mesh of the first selected object.
 * @return TriMesh of the selected object.
 */
meshSnapshot = snapshotAsMesh selection[1]

-- === Step 2: Open output file for writing ===

/**
 * @brief Path to export directory with output file name.
 */
outputPath = (getDir #export) + "/testmesh.h"

/**
 * @brief Create the file for writing mesh data.
 */
outputFile = createFile outputPath

-- === Step 3: Read mesh details ===

/**
 * @brief Total number of vertices in the mesh.
 */
vertexCount = meshSnapshot.numverts

/**
 * @brief Total number of faces in the mesh.
 */
faceCount = meshSnapshot.numfaces

-- === Step 4: Write header guard and metadata ===

format "/**\n" to:outputFile
format " * @file testmesh.h\n" to:outputFile
format " * @brief Auto-generated static mesh data from 3ds Max\n" to:outputFile
format " *\n" to:outputFile
format " * @details\n" to:outputFile
format " * Vertex count: %\n" vertexCount to:outputFile
format " * Face count: %\n" faceCount to:outputFile
format " */\n\n" to:outputFile

format "#ifndef TESTMESH_H\n#define TESTMESH_H\n\n" to:outputFile

-- Constants documentation
format "/** @brief Number of vertices in the mesh. */\n" to:outputFile
format "static const int numVertices = %;\n" vertexCount to:outputFile

format "/** @brief Number of faces in the mesh. */\n" to:outputFile
format "static const int numFaces = %;\n\n" faceCount to:outputFile

-- === Step 5: Write vertex data ===

format "/**\n" to:outputFile
format " * @brief Vertex positions.\n" to:outputFile
format " * @details Each vertex is exported as three floats: x, y, z.\n" to:outputFile
format " */\n" to:outputFile
format "static const float TeaVertices[] = {\n" to:outputFile

for v = 1 to vertexCount do (
    currentVertex = getVert meshSnapshot v
    format "\t% , % , % ,\n" (currentVertex.x as float) (currentVertex.y as float) (currentVertex.z as float) to:outputFile
)

format "};\n\n" to:outputFile

-- === Step 6: Write face index data ===

format "/**\n" to:outputFile
format " * @brief Face indices referencing TeaVertices.\n" to:outputFile
format " * @note Indices are 0-based.\n" to:outputFile
format " */\n" to:outputFile
format "static const short TeaFaceList[] = {\n" to:outputFile

for f = 1 to faceCount do (
    currentFace = getFace meshSnapshot f
    format "\t% , % , % ,\n" ((currentFace[1] - 1) as integer) ((currentFace[2] - 1) as integer) ((currentFace[3] - 1) as integer) to:outputFile
)

format "};\n\n" to:outputFile

-- === Step 7: Finalize header file ===

format "#endif // TESTMESH_H\n" to:outputFile
close outputFile
