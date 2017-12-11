#include "CCSV.h"

CCSV::CCSV(string file_name)
{
    freopen(file_name.c_str(), "rb", stdin); // abrimos el archivo con el nombre que se introdujo

    string line; // aqui vamos a guardar la linea actual que estamos recorriendo del archivo
    while(getline(cin, line)) // empezamos a leer cada linea del archivo
        m_lines.push_back(line); // metemos cada linea en el vector que la vamos a almacenar

    fclose(stdin); // cerramos el archivo
}

int CCSV::NumberLines()
{
    return m_lines.size();
}

string CCSV::GetWord(int row, int column)
{
    string line = m_lines[row]; // cargamos la linea (fila) que se nos ha pedido

    vector<string> words; // aqui vamos a guardar todas la palabras que tenga esa linea
    string word = ""; // aqui vamos a ir guardando la palabra actual

    for(int i = 0; i < line.size(); i++) // empezamos a buscar en la line cargada las palabras que tiene
    {
        if(i == line.size()-1 && word != "") // si ya llegamos al final de la linea y tenemos una labara pendiente por meter y esta no es vacia, la metemos
        {
            word += line[i];
            words.push_back(word);
            break;
        }

        if(line[i] == ',' && word != "") // para evitar que no meta palabras vacias
        {
            words.push_back(word); // metemos la palabra que acabamos de recolecta en el vector de palabras
            word = "";
        }

        if(line[i] != ',') // como es un CSV se considera una palabra hasta que se separe con una coma
            word += line[i]; // empezamos a recolectar la palabra actual
    }

    return words[column]; // retornamos la palabra que se nos habia pedido
}
