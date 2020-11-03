#include<iostream>
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
    vector<string> csv_data;
    while (getline(fin,my_str))
    {
        csv_data.push_back(my_str);
    }
    fin.close();
    return csv_data;
}

/*
This method is used to seperate the data to several array.
It used two dimentions pointer to realize the multidimention array.
*/

vector<vector<string>> DataPreHandle(vector<string> data,int data_row)
{
    vector<vector<string>> my_data;
    vector<string> data_bin;
    string str;
    for(int row=1;row<=data_row;row++)  //get the second line 
    {
        int index=0; 
        for(int j=0;j<data[row].size();j++)
        {
            if (data[row][j]!=',')
            {
                str = str+data[row][j];
            }
            else
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }           
            if(j==data[row].size()-1) 
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }
        }        
        my_data.push_back(data_bin);
        data_bin.clear();
    }
    return  my_data;
}
/*
this method is dot product function
*/
double DotProduct(vector<double> a,vector<double> b)
{
    double result=0.0;
    if(a.size()==b.size())
    {
        for(int i =0; i <a.size();i++)
        {
            result = result+a[i]*b[i];
        }
    }
    return result;
}
double DotProduct(double a,double b)
{
    return a*b;
}
/*
This method is sign function. Each time call this func, it will return the result of one row of data.
*/
double Sign(vector<double> weight,double bias,vector<double> x_train_data)
{
    double y=0.0;
    y=y+DotProduct(x_train_data,weight)+bias;
    return y;
}
/*
This method is gradient descent
*/
vector<double> OriginalGradientDescent(double learning_rate,vector<vector<double>> x_train_data,vector<double> y_train_data)
{
    //initialize weights as 1 and bias as 0
    double bias = 0;
    vector<double> weight;
    for (int i = 0; i < x_train_data[0].size(); i++)
    {
         weight.push_back(1);
    }
    cout<<"weight size:"<<weight.size()<<endl;
    //start to calculate the gradient
    bool is_wrong = false;
    
    while(!is_wrong)
    {
        int wrong_count=0;
        for(int row =0;row<x_train_data.size();row++)
        {
            double flag;
            flag = y_train_data[row]*Sign(weight,bias,x_train_data[row]);//Loss function: y label and y predict should be positive number, if the instance was classified correctly. otherwise the weight and bias need to be update.
            if (flag<=0)
            {
                for(int col=0;col < x_train_data[0].size();col++)
                {      
                    weight[col]=weight[col]+learning_rate*x_train_data[row][col]*y_train_data[row]; // udpating the weight gradient 
                    bias=bias+learning_rate*y_train_data[row];// updating the bias gradient
                    wrong_count++;
                }
            }
        }
        if (wrong_count ==0)
        {
            is_wrong = true;
        }
    }
    vector<double> parameters;
    for (int i = 0;i<weight.size()+1;i++)
    {
        if(i!=weight.size())
        {
            parameters.push_back(weight[i]);
        }
        else
        {
            parameters.push_back(bias);
        }
    }
    return parameters;
}
/*
This method is  original perceptron algorithm
*/
void PerceptronOriginal(string path, int data_row, int data_column, double learning_rate=0.1,double train_data_percent=1)// parameters without default value has to be put at the first position.
{
    //Spliting data as train and prediction data
    vector <string> iris_data = ReadCsvFile(path);
    vector<vector<string>> my_data = DataPreHandle(iris_data,data_row);
    cout<<"my_data row"<<my_data.size()<<endl;
    cout<<"my_data column"<<my_data[0].size()<<endl;
    vector<vector<double>> x_train_data;
    vector<double> y_train_data;
    vector<double> data_bin;

    //set X dataset and convert Y label as -1,1
    for (int row =0;row<(data_row*train_data_percent);row++)
    {
        for(int column =1;column<data_column;column++)// get from second column
        {
            data_bin.push_back(atof(my_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        data_bin.clear();
        if(my_data[row][data_column]=="Iris-setosa")
        {
            y_train_data.push_back(1);
        }
        else
        {
            y_train_data.push_back(-1);
        } 
    }

    // use gradient descent to get weights and bias
    vector<double> parameters=OriginalGradientDescent(learning_rate,x_train_data,y_train_data);
    for(int i=0;i<parameters.size();i++)
    {
        cout<<"parameter-"<<i<<":"<<parameters[i]<<endl;
    }
}

/*
This is main method
*/
int main()
{
    string path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    PerceptronOriginal(path,150, 5);
    return 0;
}







// string** DataPreHandle(vector<string> data,int data_row,int data_col)
// {
//     static string ** my_data= new string *[data_row];   // need to declare a static pointer space avoiding the system auto releasing  memory
//     string str;
//     string col_container[data_col];
    
//     for(int row=0;row<data_row;row++)  //split string by comma,by getting string length of each column
//     {
//         my_data[row] = new string [data_col];  //assign row pointer space for each column
//         int index=0; 
//         for(int j=0;j<data[row].size();j++)
//         {
//             if (data[row][j]!=',')
//             {
//                 str = str+data[row][j];
//             }
//             else
//             {
//                 col_container[index] = str; 
//                 index++;
//                 str = "";
//             }           
//             if(j==data[row].size()-1) 
//             {
//                 col_container[index] = str; 
//                 index++;
//                 str = "";
//             }
//         }        
//         for(int col=0;col<data_col;col++)
//         { 
//             my_data[row][col] = col_container[col];
//         }
//     }
//     return  my_data;
// }
