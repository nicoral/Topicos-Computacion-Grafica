#include "head.h"


/*
Integrantes:
-Andre Mogrovejo Martinez
-Nicolas Jimenez Artica
-Jesus Lazo Quevedo
*/

int main() {
    //carga del archivo e inicializacion de estructura Bitmap
    BitMap ejemplo_bmp("gatote.bmp"); 

    //ejemplo get vector <r,g,b> para el pixel (1,1)
    vector<unsigned int> example_vector = ejemplo_bmp.getPixel(1,1); 

    ejemplo_bmp.makeCopy("gatote2.bmp");
    //ejemplo_bmp.dispPixelData();
    
    int altura = ejemplo_bmp.height();
    int ancho = ejemplo_bmp.width();
    vector<unsigned int> RGB;
    int griss = 0;
    for(int j= 0; j<(ancho/2);j++){
      for(int i=0;i<(altura/2);i++)
      {
        RGB = ejemplo_bmp.getPixel(i,j);
        griss = int(RGB[0]*0.299)%255 + int(RGB[1]*0.114)%255 + int(RGB[2]*0.587)%255;
        ejemplo_bmp.writePixel(i,j,griss,griss,griss);
      }
    }
  return 0;
}