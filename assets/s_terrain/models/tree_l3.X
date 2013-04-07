xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}


Material PDX01_-_Default {
 1.000000;1.000000;1.000000;1.000000;;
 3.200000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "plant3.png";
 }
}

Frame Cylinder001 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh  {
  13;
  0.249372;0.000000;-0.142543;,
  -0.001975;0.000000;0.276466;,
  -0.240355;0.000000;-0.153487;,
  0.249372;0.999900;-0.142543;,
  -0.001975;0.999900;0.276466;,
  -0.240355;0.999900;-0.153487;,
  0.811761;1.242930;-0.483588;,
  0.005291;1.242930;0.823403;,
  -0.810312;1.242930;-0.499560;,
  -0.000000;4.999499;-0.000000;,
  0.249372;0.000000;-0.142543;,
  0.249372;0.999900;-0.142543;,
  0.811761;1.242930;-0.483588;;
  9;
  3;1,3,4;,
  3;3,1,0;,
  3;2,4,5;,
  3;4,2,1;,
  3;10,5,11;,
  3;5,10,2;,
  3;6,9,7;,
  3;7,9,8;,
  3;8,9,12;;

  MeshNormals  {
   10;
   -0.017037;0.000000;0.999855;,
   0.875625;0.000000;-0.482992;,
   -0.017037;0.000000;0.999855;,
   0.875625;0.000000;-0.482992;,
   -0.855930;0.000000;-0.517091;,
   -0.855930;0.000000;-0.517091;,
   0.847443;0.243832;-0.471579;,
   -0.000605;0.999906;0.013717;,
   0.000448;0.214108;0.976810;,
   -0.841996;0.245506;-0.480385;;
   9;
   3;0,1,2;,
   3;1,0,3;,
   3;4,2,5;,
   3;2,4,0;,
   3;3,5,1;,
   3;5,3,4;,
   3;6,7,8;,
   3;8,7,9;,
   3;9,7,6;;
  }

  MeshMaterialList  {
   1;
   9;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { PDX01_-_Default }
  }

  MeshTextureCoords  {
   13;
   0.468801;0.501505;,
   0.321826;0.501505;,
   0.222720;0.501505;,
   0.468801;0.390150;,
   0.321826;0.390150;,
   0.222720;0.390150;,
   0.467687;0.342933;,
   0.320712;0.342933;,
   0.221606;0.342933;,
   0.215763;0.011589;,
   0.026482;0.501505;,
   0.026482;0.390150;,
   0.025367;0.342933;;
  }
 }
}