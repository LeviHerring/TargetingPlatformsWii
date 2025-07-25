/**
 * @file export_mesh_snapshot.ms
 * @brief Exports a snapshot of the first selected object as a static mesh header file for use in C/C++ applications.
 *
 * @details
 * Naming conventions:
 * - camelCase for variables and function-like constructs (e.g., exportMeshSnapshot, vertexCount)
 * - PascalCase for constants and exported arrays (e.g., TeaVertices, TeaFaceList)
 */

--// === Step 1: Get the mesh snapshot of the first selected object ===

/**
 * @brief Capture the snapshot mesh of the first selected object.
 * @return TriMesh of the selected object.
 */
meshSnapshot = snapshotAsMesh selection[1]

--// === Step 2: Open output file for writing ===

/**
 * @brief Path to export directory with output file name.
 */
outputPath = (getDir #export) + "/testmesh.h"

/**
 * @brief Create the file for writing mesh data.
 */
outputFile = createFile outputPath

--// === Step 3: Read mesh details ===

/**
 * @brief Total number of vertices in the mesh.
 */
vertexCount = meshSnapshot.numverts

/**
 * @brief Total number of faces in the mesh.
 */
faceCount = meshSnapshot.numfaces

--// === Step 4: Write header guard and metadata ===

/**
 * @brief Start the header guard.
 */
format "#ifndef TESTMESH_H\n#define TESTMESH_H\n\n" to:outputFile

/**
 * @brief Add vertex and face count as comments.
 */
format "// Number of vertices: %\n" vertexCount to:outputFile
format "// Number of faces: %\n\n" faceCount to:outputFile

/**
 * @brief Write vertex and face count as C constants.
 */
format "static const int numVertices = %;\n" vertexCount to:outputFile
format "static const int numFaces = %;\n\n" faceCount to:outputFile

--// === Step 5: Write vertex data ===

/**
 * @brief Begin declaration of vertex array.
 */
format "static const float teaVertices[] = {\n" to:outputFile

/**
 * @brief Export all vertices as float values.
 */
for v = 1 to vertexCount do (
    currentVertex = getVert meshSnapshot v
    format "\t% , % , % ,\n" (currentVertex.x as float) (currentVertex.y as float) (currentVertex.z as float) to:outputFile
)

/**
 * @brief End of vertex array.
 */
format "};\n\n" to:outputFile

--// === Step 6: Write face index data ===

/**
 * @brief Begin declaration of face index array.
 */
format "static const short teaFaceList[] = {\n" to:outputFile

/**
 * @brief Export all face indices (0-based).
 */
for f = 1 to faceCount do (
    currentFace = getFace meshSnapshot f
    format "\t% , % , % ,\n" ((currentFace[1] - 1) as integer) ((currentFace[2] - 1) as integer) ((currentFace[3] - 1) as integer) to:outputFile
)

/**
 * @brief End of face index array.
 */
format "};\n\n" to:outputFile

--// === Step 7: Finalize header file ===

/**
 * @brief Close the header guard.
 */
format "#endif // TESTMESH_H\n" to:outputFile

/**
 * @brief Close the file stream.
 */
close outputFile
