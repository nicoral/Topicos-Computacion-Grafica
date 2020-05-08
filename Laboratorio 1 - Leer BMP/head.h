#ifndef BMP_H
#define BMP_H

#include <iostream>
#include <vector>
#include <stdio.h>      
#include <math.h>
#include <fstream>

using namespace std;
class BitMap {

    private:
        unsigned char m_bmpHeader[14];
        unsigned int m_pixelArrayOffset;
        unsigned char m_bmpInfoHeader[40];

        int m_height;
        int m_width;
        int m_bitsPerPixel;

        int m_rowSize;
        int m_pixelArraySize;

        unsigned char* m_pixelData;

        char * m_copyname;
        const char * m_archivo;
    public:
        BitMap(const char * archivo);
        ~BitMap();
        vector<unsigned int> getPixel(int i,int j);

        void makeCopy(char * archivo);
        void writePixel(int i,int j, int R, int G, int B);

        void swapPixel(int i, int j, int i2, int j2);

        void dispPixelData();

        int width() {return m_width;}
        int height() {return m_height;}

        int vd(int i, int j);
        int hd(int i, int j);

        bool isSorted();
};

BitMap::BitMap( const char * archivo) {

    using namespace std;

    m_archivo = archivo;
    //Comprobando si el archivo puede abrirse
    ifstream inf(archivo);
    if(!inf) {
        cerr<<"No se pudo abrir el archivo: "<<archivo<<"\n";
    }
    //Inicializacion de bmpHeader
    unsigned char a;
    for(int i =0;i<14;i++) {
        inf>>hex>>a;
        m_bmpHeader[i] = a;
    }//Comprobando que es un archivo BM
    if(m_bmpHeader[0]!='B' || m_bmpHeader[1]!='M') {
        cerr<<"Error en la cabecera, deberia iniciar con 'BM'\n";
    }
    //calculo de offset para soporte de funciones
    unsigned int * array_offset_ptr = (unsigned int *)(m_bmpHeader + 10);
    m_pixelArrayOffset = *array_offset_ptr;

    //comprobando bits de cabecera de los datos de la imagen
    if( m_bmpHeader[11] != 0 || m_bmpHeader[12] !=0 || m_bmpHeader[13] !=0 ) {
        cerr<< "Algo esta mal. bmp.h("<<__LINE__<<")\n";
    }
    //Almacenando informacion del header bmp
    for(int i=0;i<40;i++) {
        inf>>hex>>a;
        m_bmpInfoHeader[i]=a;
    }
    //punteros a altura y ancho de los pixeles de la imagen
    int * width_ptr = (int*)(m_bmpInfoHeader+4);
    int * height_ptr = (int*)(m_bmpInfoHeader+8);
    //valores enteros de ancho y alto
    m_width = *width_ptr;
    m_height = *height_ptr;
    //mostrando valores en pantalla
    printf("W: %i, H: %i", m_width, m_height);
    //Comprobando que el archivo sea de 24bits
    m_bitsPerPixel = m_bmpInfoHeader[14];
    if(m_bitsPerPixel!=24) {
        cerr<<"Este programa funciona solo para archivos de 24bits\n";
    }//Comprobando que no haya comprension en el archivo (bit 30)
    int compressionMethod = m_bmpInfoHeader[16];
    if(compressionMethod!=0) {
        cerr<<"Hay un problema con el metodo de compresion, no podemos manejarla\n";
        cerr<<__LINE__<<"\n";
    }
    //Calculo del tamaño de cada fila
    m_rowSize = int( floor( (m_bitsPerPixel*m_width + 31.)/32 ) ) *4;
    m_pixelArraySize = m_rowSize* abs(m_height);//Calculo del tamaño de un pixel
    //Inicializacion de vector pixel data que contiene info rgb del px
    m_pixelData = new unsigned char [m_pixelArraySize];

    inf.seekg(m_pixelArrayOffset,ios::beg);
    for(int i=0;i<m_pixelArraySize;i++) {
        inf>>hex>>a;
        m_pixelData[i]=a; 
    }
}
//Destructor
BitMap::~BitMap() {
    delete[] m_pixelData;
}
//Funcion que muestra la data de los pixeles
void BitMap::dispPixelData() {
    for(int i=0;i<m_pixelArraySize;i++) {
        cout<<(unsigned int)m_pixelData[i]<<" ";   
    }
    cout<<"\n";
}

//funcion que obtiene un pixel de acuerdo a su index x,y
// salida dada en orden rgb
vector<unsigned int> BitMap::getPixel(int x, int y) {
    if(x<m_width && y<m_height) {
        vector<unsigned int> v;//inciializamos vector con 0,0,0
        v.push_back(0);
        v.push_back(0);
        v.push_back(0);

        //y = m_height -1- y; //voltear cosas
        //cout<<"y: "<<y<<" x: "<<x<<"\n";
        v[0] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+2 ] ); //red
        v[1] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+1 ] ); //greed
        v[2] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+0 ] ); //blue
        return v;
    }
    else {cerr<<"Error de index\n";cerr<<"X: "<<x<<" Y: "<<y<<"\n";}
}
//funcion que copia el archivo 
void BitMap::makeCopy(char * archivo) {
    ofstream copyfile(archivo);
    ifstream infile(m_archivo);
    m_copyname = archivo;

    unsigned char c;
    while(infile) {
        infile>>c;
        copyfile<<c;
    }
}

// modificar un pixel del archivo
void BitMap::writePixel(int x,int y, int R, int G, int B) {
    fstream file(m_archivo);
    y = m_height -1- y; // voltear cosas
    int blueOffset = m_pixelArrayOffset+m_rowSize*y+3*x+0;

    // escritura en el archivo
    file.seekg(blueOffset,ios::beg);
    file<< (unsigned char)B;
    file.seekg(blueOffset+1,ios::beg);
    file<< (unsigned char)G;
    file.seekg(blueOffset+2,ios::beg);
    file<< (unsigned char)R;

    //edita el dato en el array pixel
    m_pixelData[m_rowSize*y+3*x+2] = (unsigned char)R;
    m_pixelData[m_rowSize*y+3*x+1] = (unsigned char)G;
    m_pixelData[m_rowSize*y+3*x+0] = (unsigned char)B;
}

// modifica el archivo
void BitMap::swapPixel(int i, int j, int i2, int j2) {
    vector<unsigned int> p1 = (*this).getPixel(i,j);

    vector<unsigned int> p2 = (*this).getPixel(i2,j2);

    (*this).writePixel(i,j,p2[0],p2[1],p2[2]);
    (*this).writePixel(i2,j2,p1[0],p1[1],p1[2]);

}
#endif