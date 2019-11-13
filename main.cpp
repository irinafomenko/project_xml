#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"
#include <curl/curl.h>
#pragma comment(lib,"libcurl")

using namespace rapidxml;
using namespace std;

//объявляем буфер, для хранения возможной ошибки, размер определяется в самой библиотеке
static char errorBuffer[CURL_ERROR_SIZE];
//объялвяем буфер принимаемых данных
static string buffer_xml;
//необходимые CURL объекты
CURL *curl;

//функция обратного вызова
static int writer(char *data, size_t size, size_t nmemb, string *buffer)
{
    //переменная - результат, по умолчанию нулевая
    int result = 0;
    //проверяем буфер
    if (buffer != NULL)
    {
        //добавляем к буферу строки из data, в количестве nmemb
        buffer->append(data, size * nmemb);
        //вычисляем объем принятых данных
        result = size * nmemb;
    }
    //вовзращаем результат
    return result;
}

void save_file_xml()
{
    std::ofstream file_xml;
    file_xml.open("file.xml");
    file_xml << buffer_xml;
    file_xml.close();
}

void download_xml()
{
    //необходимые CURL объекты
    //CURL *curl;
    CURLcode result;
    //инициализируем curl
    curl = curl_easy_init();
    //проверяем результат инициализации
    if (curl)
    {
        //задаем все необходимые опции
        //определяем, куда выводить ошибки
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        //задаем опцию - получить страницу по адресу http://google.com
        curl_easy_setopt(curl, CURLOPT_URL, "https://gist.githubusercontent.com/JSchaenzle/2726944/raw/c0c6f29f4b3161656ab58d4cae2b8e92239c79a8/beerJournal.xml");
        //указываем прокси сервер
        curl_easy_setopt(curl, CURLOPT_PROXY, "proxy:8080");
        //задаем опцию отображение заголовка страницы
        //curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        //указываем функцию обратного вызова для записи получаемых данных
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        //указываем куда записывать принимаемые данные
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer_xml);
        //запускаем выполнение задачи
        result = curl_easy_perform(curl);
        //проверяем успешность выполнения операции
        /*
        if (result == CURLE_OK)
        {
            //выводим полученные данные на стандартный вывод (консоль)
            cout << buffer_xml << "\n";
        }
        //else
         */
        save_file_xml();
        if(result != CURLE_OK)
        {
            //выводим сообщение об ошибке
            cout << "Ошибка! " << errorBuffer << endl;
        }
    }
    //завершаем сессию
    curl_easy_cleanup(curl);
}

void parsing_xml()
{
    cout << "Parsing my beer journal..." << endl;
    xml_document<> doc;
    xml_node<> * root_node;
    // Read the xml file into a vector
    ifstream theFile ("file.xml");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(&buffer[0]);
    // Find our root node
    root_node = doc.first_node("MyBeerJournal");
    // Iterate over the brewerys
    for (xml_node<> * brewery_node = root_node->first_node("Brewery"); brewery_node; brewery_node = brewery_node->next_sibling())
    {
        printf("I have visited %s in %s. ",
               brewery_node->first_attribute("name")->value(),
               brewery_node->first_attribute("location")->value());
        // Interate over the beers
        for(xml_node<> * beer_node = brewery_node->first_node("Beer"); beer_node; beer_node = beer_node->next_sibling())
        {
            printf("On %s, I tried their %s which is a %s. ",
                   beer_node->first_attribute("dateSampled")->value(),
                   beer_node->first_attribute("name")->value(),
                   beer_node->first_attribute("description")->value());
            printf("I gave it the following review: %s", beer_node->value());
        }
        cout << endl;
    }
}

int main(void)
{
    download_xml();
    parsing_xml();
    return 0;
}