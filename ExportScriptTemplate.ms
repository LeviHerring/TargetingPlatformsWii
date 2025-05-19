tmesh = snapshotAsMesh selection[1]
out_file = createfile ((GetDir #export) + "/testmesh.dat")

-- Write the vertex array declaration
format "s16 vertices[] ATTRIBUTE_ALIGN(32) = {\n" to:out_file

-- Export vertex data
for v = 1 to tmesh.numverts do (
    vert = getVert tmesh v
    format "\t% , % , % ,\n" (vert.x as integer) (vert.y as integer) (vert.z as integer) to:out_file
)

format "};\n\n" to:out_file

-- Write face list
format "s16 faceList[] ATTRIBUTE_ALIGN(32) = {\n" to:out_file

for f = 1 to tmesh.numfaces do (
    face = getFace tmesh f
    format "\t% , % , % ,\n" (face[1] - 1 as integer) (face[2] - 1 as integer) (face[3] - 1 as integer) to:out_file
)

-- Close the array
format "};\n" to:out_file

close out_file
