#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"
#include <curl/curl.h>
#include <queue>

#pragma comment(lib,"libcurl")

using namespace rapidxml;
using namespace std;

//объявляем буфер, для хранения возможной ошибки, размер определяется в самой библиотеке
static char errorBuffer[CURL_ERROR_SIZE];
//объялвяем буфер принимаемых данных
static string buffer_xml;
//необходимые CURL объекты
CURL *curl;
bool flag_not_found = false;
//xml file
xml_document<> doc;

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
        //curl_easy_setopt(curl, CURLOPT_URL, "https://gist.githubusercontent.com/JSchaenzle/2726944/raw/c0c6f29f4b3161656ab58d4cae2b8e92239c79a8/beerJournal.xml");
        //curl_easy_setopt(curl, CURLOPT_URL, "https://www.w3schools.com/XML/cd_catalog.xml");
        curl_easy_setopt(curl, CURLOPT_URL, "http://producthelp.sdl.com/sdl%20trados%20studio/client_en/sample.xml");
        //curl_easy_setopt(curl, CURLOPT_URL, "https://www.javatpoint.com/xmlpages/books.xml");
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

void print_result(xml_node<> * node)
{
    cout << endl;
    cout << "Name tag: " << node->name() << endl;
    cout << "Value tag: " << node->value() << endl;
    for (xml_attribute<> * att = node->first_attribute(); att; att = att->next_attribute())
    {
        cout << "Name attribute: " << att->name() << endl;
        cout << "Value attribute: " << att->value() << endl;
    }
    flag_not_found = true;
}

void search_with_value(string name_tag, string value_tag)
{
    xml_node<> * root_node;
    root_node = doc.first_node();
    if(root_node->name() == name_tag && root_node->value() == value_tag) {print_result(root_node);}
    for (xml_node<> * child_node = root_node->first_node(); child_node; child_node = child_node->next_sibling())
    {
        if(child_node->name() == name_tag && child_node->value() == value_tag) {print_result(child_node);}
        for(xml_node<> * children_node = child_node->first_node(); children_node; children_node = children_node->next_sibling())
        {
            if(children_node->name() == name_tag && children_node->value() == value_tag) {print_result(children_node);}
        }
        //cout << endl;
    }
}
void search_without_value(string name_tag)
{
    xml_node<> * root_node;
    root_node = doc.first_node();
    if(root_node->name() == name_tag) {print_result(root_node);}
    for (xml_node<> * child_node = root_node->first_node(); child_node; child_node = child_node->next_sibling())
    {
        if(child_node->name() == name_tag) {print_result(child_node);}
        for(xml_node<> * children_node = child_node->first_node(); children_node; children_node = children_node->next_sibling())
        {
            if(children_node->name() == name_tag) {print_result(children_node);}
        }
        //cout << endl;
    }
}

void parsing_xml()
{
    string name_tag, value_tag;
    cout << "Parsing..." << endl;
    // Read the xml file into a vector
    ifstream theFile ("file.xml");
    //ifstream theFile ("C:/Users/ifomenko/CLionProjects/project_xml/beerJournal.xml");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(&buffer[0]);
    //doc.parse<parse_declaration_node | parse_no_data_nodes>(&buffer[0]);
    // Find our root node
    cout << "Enter name tag: ";
    cin >> name_tag;
    cin.ignore(32767, '\n'); // удаляем символ новой строки из входного потока данных
    cout << "Enter value tag: ";
    //cin >> value_tag;
    getline(cin, value_tag);

    if(!value_tag.empty()) {search_with_value(name_tag, value_tag);}
    else {search_without_value(name_tag);}

    if(flag_not_found == false) {cout << endl << "Not found!" << endl;}
}

int main(void)
{
    download_xml();
    parsing_xml();
    return 0;
}