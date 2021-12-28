#include<iostream>
#include<fstream>
//#include <wchar.h>
//#include <locale.h>
//#include <stdlib.h>

using namespace std;

const int TAMANNO_PAQUETE = 26;

/*
string iso_8859_1_to_utf8(string &str)
{
    string strOut;
    for (string::iterator it = str.begin(); it != str.end(); ++it)
    {
        uint8_t ch = *it;
        if (ch < 0x80) {
            strOut.push_back(ch);
        }
        else {
            strOut.push_back(0xc0 | ch >> 6);
            strOut.push_back(0x80 | (ch & 0x3f));
        }
    }
    return strOut;
}
*/

int main(){
    //setlocale(LC_ALL, "");
    string nombrefichero = "tweets-sinProcesar.csv";
    string salida = "salida.txt";
    ifstream f;
    ofstream g;
    string basura;
    string fechaaux;
    string aplicacion;
    string autor;
    string tweetaux;
    bool vaBien;
    char charaux, charaux1;
    string chariso;
    f.open(nombrefichero);
    g.open(salida);
    getline(f,basura);


    for(unsigned int i= 0; i<TAMANNO_PAQUETE; i++){

        //getline(f, fechaaux, ';');
        charaux = ' ';
        vaBien = true;

        fechaaux = "";  
        while(charaux!=';' && charaux !='\n'){
            f.get(charaux);
            fechaaux = fechaaux + charaux;
        }
        
        vaBien = charaux !='\n';
        

        //getline(f, aplicacion, ';');
        aplicacion = "";  
        while(charaux!=';' && charaux !='\n' && vaBien){
            f.get(charaux);
            aplicacion = aplicacion + charaux;
        }
        
        vaBien = charaux !='\n';
        
        //getline(f, basura, ';');
        autor = "";  
        while(charaux!=';' && charaux !='\n' && vaBien){
            f.get(charaux);
            autor = autor + charaux;
        }
        
        vaBien = charaux !='\n';



        if(vaBien){
            g << fechaaux  << aplicacion  << autor;
    
            do{
                f.get(charaux);
                if((charaux >= 'a' && charaux <= 'z')||(charaux >= '@' && charaux <= 'Z')||(charaux >= ','&&charaux <= ';' )||
                charaux == ' ' || charaux == '_' || charaux == '#' || charaux == -61 ){

                    if(charaux == -61){
                        
                        charaux1 = charaux;

                        f.get(charaux);
                        if(charaux == -95 || charaux == -87 || charaux == -83 || charaux == -79 || charaux == -111  || charaux == -77 || 
                           charaux == -70 || charaux == -127 || charaux == -119 || charaux == -115 || charaux == -109 || charaux == -102){

                            g<< charaux1 << charaux;

                        }


                    }
                    else{  
                        g << charaux;
                    }

                }

            }while(charaux!='\n');
            g << endl;
        
        }
    }

    f.close();
    g.close();


    return 0;



}