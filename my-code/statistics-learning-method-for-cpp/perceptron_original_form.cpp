#include <iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
using namespace std;
/*
this method is used for reading csv file to a vector class 
*/
vector<string> ReadCsvFile(string path)
{
    ifstream fin (path);
    string my_str;
    vector<string> my_data;
    while (getline(fin,my_str))
    {
        my_data.push_back(my_str);
    }
    fin.close();
    return my_data;
}

/*
this method is used to seperate the data to several array
*/

string* DataPreHandle(vector<string> data)
{
    int data_amount=1;
    for(int i=0;i<data[0].size();i++)
    {    
        if(data[0][i]==',')
        {
            data_amount++;
        }
    }
    static string my_data[data_amount][data.size()];
    for(int i=0;i<data_amount;i++)
    {
        for(int j=0;j<data[i].size();j++)
        {
             string str = "";
             if (data[i][j]!=',')
             {
                 str = str+data[i][j];
             }
             else
             {
                 my_data[i][j] = str; 
             }
        }
    }
    return my_data;

}

/*
 this is main method
*/

int main()
{
    string path="/home/fengchao/Nutstore Files/代码/统计学习方法/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    vector <string> iris_data = ReadCsvFile(path);
    string* my_data;
    my_data = DataPreHandle(iris_data);
    for (int i =0; i<6;i++)
    {
        for(int j = 0; j<my_data[0].size();j++)
        {
            cout<<my_data[i][j];
        }
        cout << endl;
    }
    return 0;
}
