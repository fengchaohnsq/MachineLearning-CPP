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
double DotProduct(vector<double> a,double b)
{
    double result=0.0;
    for(int i =0; i <a.size();i++)
    {
        result = result+a[i]*b;
    }
    return result;
}
/*
this method calcualtes the gram matrix, the high dimention X multiplication need dot product to generate a single value.
*/
vector<vector<double>> Gramatrix(vector<vector<double>> x_train_data)
{
    vector<vector<double>> gram_matrix;
    vector<double> gram_bin;
    for(int row =0;row<x_train_data.size();row++)
    {
        for(int row1=0;row1<x_train_data.size();row1++)
        {
            gram_bin.push_back(DotProduct(x_train_data[row],x_train_data[row1]));
        }
        gram_matrix.push_back(gram_bin);
        gram_bin.clear();
    }
    return gram_matrix;
}
/*
This method is sign function. Each time call this func, it will return the result of one row of data.
*/
double Sign(vector<double> alpha,double bias,vector<vector<double>> x_train_data,vector<double> y_train_data,vector<vector<double>> gram_matrix,int data_row)
{
    double y=0.0;
    for(int j = 0; j < x_train_data.size();j++)
    {
        y=y+alpha[j]*y_train_data[j]*gram_matrix[data_row][j];
    }

    return y;
}
/*
This method is gradient descent
*/
vector<double> DualGradientDescent(double learning_rate,vector<vector<double>> x_train_data,vector<double> y_train_data)
{
    //initialize weights as 0 and bias as 0
    double bias = 0;
    vector<double> alpha;
    for (int i = 0; i < x_train_data.size(); i++)
    {
        alpha.push_back(0);
    }
    //start to calculate the gradient
    bool is_wrong = false;
    // ger X train data gram matrix
    vector<vector<double>> gram_matrix=Gramatrix(x_train_data);
    while(!is_wrong)
    {
        int wrong_count=0;
        
        for(int row =0;row<x_train_data.size();row++)
        {
            double flag;
            flag= y_train_data[row]*Sign(alpha,bias,x_train_data,y_train_data,gram_matrix,row);
            if(flag<=0)
            {
                alpha[row]=alpha[row]+learning_rate;// alphai = alphai + leaning rate
                bias= bias+alpha[row]*y_train_data[row];//bias = bias + learning rate * y_label
            }
        }
        if (wrong_count ==0)
        {
            is_wrong = true;
        }
    }
    vector<double> parameters;
    for (int i = 0;i<alpha.size()+1;i++)
    {
        if(i!=alpha.size())
        {
            parameters.push_back(alpha[i]);
        }
        else
        {
            parameters.push_back(bias);
        }
    }
    return parameters;
}
/*
this method is dual perceptron algorithm
*/
void PerceptronDual(string path, int data_row, int data_column, double learning_rate=0.1,double train_data_percent=1)
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
    for (int row =0;row<(data_row*train_data_percent);row++)//get row from second line
    {
        for(int column =1;column<data_column;column++)
        {
            data_bin.push_back(atof(my_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        data_bin.clear();
        // set Iris-setosa as 1, other species as -1
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
    vector<double> parameters=DualGradientDescent(learning_rate,x_train_data,y_train_data);
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
    PerceptronDual(path,150, 5);

    return 0;
}