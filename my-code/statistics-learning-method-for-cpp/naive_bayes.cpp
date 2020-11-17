#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
#include <algorithm>
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
vector<vector<vector<string>>> DataPreHandle(string path,int data_row)
{
    vector <string> iris_data = ReadCsvFile(path);
    vector<string> data_bin;
    vector<vector<string>> my_data_bin;
    vector<vector<string>> data_bin1;
    vector<vector<vector<string>>> my_data;
    string str;
    for(int row=0;row<=data_row;row++) 
    {
        int index=0; 
        for(int j=0;j<iris_data[row].size();j++)
        {
            if (iris_data[row][j]!=',')
            {
                str = str+iris_data[row][j];
            }
            else
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }           
            if(j==iris_data[row].size()-1) // the last data
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }
        }        
        my_data_bin.push_back(data_bin);
        data_bin.clear();
    }
    //set Y label
    for (int row =0;row<data_row;row++)//get row from first line which include the id
    {
        data_bin.push_back(my_data_bin[row][my_data_bin[row].size()-1]);  //get the last data of each row, which is the label data.
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    //set X data
    for (int row =0;row<data_row;row++)//get row from first line which include the id
    {
        for(int column =0;column<my_data_bin[row].size()-1;column++)
        {
            data_bin.push_back(my_data_bin[row][column]);  //get the data of each row except the last data,which is the x data.
        }
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    return  my_data;
}
/*
This method is used to converse the string type data to double and spliting data to x data and y label.
*/
void StringToDouble(vector<vector<vector<string>>> my_data,vector<string> label_class,vector<vector<double>> &x_data,vector<string> &label)
{
    vector<vector<string>> y_bin=my_data[0];
    vector<vector<string>> x_bin=my_data[1];
    vector<double>data_bin;
    //set X dataset 
    for (int row =1;row<x_bin.size();row++)//exclude column name
    {
        for(int column =1;column<x_bin[0].size();column++)//exclude Id column
        {
            data_bin.push_back(atof(x_bin[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_data.push_back(data_bin);
        data_bin.clear();  
    }
    //set Y label
    for (int row =1;row<y_bin.size();row++) //exclude column name
    {
       label.push_back(y_bin[row][0]);//class column
    }
}
/*
This method gets the mean value of each dimention of X data in each label
*/
double MeanValue(vector<double> x_data)
{
    double sum=0.0;
    for(int i=0 ;i<x_data.size();i++)
    {
        sum=sum+x_data[i];
    }
    return sum/x_data.size();
}
/*
This method gets the variance of each dimentaion of X data in each label
*/
double Variance(vector<double> x_data)
{
    double mean_value = MeanValue(x_data);
    double sum=0.0;
    for(int i=0 ;i<x_data.size();i++)
    {
        sum= sum+pow(x_data[i]-mean_value,2);
    }
    return sum/x_data.size();
}
/*
this method transpose the matrix.
*/
vector<vector<double>> Matrix_Transpose(vector<vector<double>> data)
{
    vector<double>bin;
    vector<vector<double>> transpose_data;
    for(int col=0;col<data[0].size();col++)
    {
        for(int row=0;row<data.size();row++)
        {
            bin.push_back(data[row][col]);
        }
        transpose_data.push_back(bin);
        bin.clear();
    }
    return transpose_data;
}
/*
generating bayes classifier 
P(Y|X)P(X)= P(X|Y)P(Y)=P(XY)
P(Y|X)=P(X|Y)P(Y)/P(X)
Classifirt is to find the bigest P(Y|X),it means to find the biggest P(X|Y)P(Y).
Method will return all of P(Yk),which k is the number of labels, 
and it also returns all of P(Xi|Yk) in k labels, which i is the number of features.
Because the feature is continuous data,so we need a scale method which classifies those data.
This method is gaussian probability density function. 
By getting all datas' each labels' each dimention mean value & variance,to get the probability of each datas' dimention in the label. 
The probability of each dimention in one row data are independent with each other.
*/
void GaussianBayesClassifier(vector<vector<double>> x_data,vector<string> label,vector<string>label_class,vector<double> input)
{
    vector<double> P_Yk;
    vector<vector<double>> P_Xi_Yk;
    double label_num=label.size();
    vector<vector<vector<double>>> classified_data;
    vector<vector<double>> data_bin;
    // Split data by y label class
    for(int i=0; i<label_class.size();i++)
    {
        P_Yk.push_back(count(label.begin(),label.end(),label_class[i])/label_num);
        for(int j=0;j<label.size();j++)
        {
            if(label_class[i]==label[j])
            {
                data_bin.push_back(x_data[j]);
            }
        }
        classified_data.push_back(data_bin);
        data_bin.clear();
    }
    // Calculate mean value and variance of each dimention of data which is classified by data label.
    vector<vector<double>>mean_value;
    vector<vector<double>>variance;
    vector<double>bin1;
    vector<double>bin2;
    for(int data_batch=0; data_batch<classified_data.size();data_batch++)
    {
        vector<vector<double>> transpose_data=Matrix_Transpose(classified_data[data_batch]);
        for(int dim=0;dim<transpose_data.size();dim++)
        {
            bin1.push_back(MeanValue(transpose_data[dim]));
            bin2.push_back(Variance(transpose_data[dim]));
        }
        mean_value.push_back(bin1);
        variance.push_back(bin2);
        bin1.clear();
        bin2.clear();
    }
    // Calculate the gaussian probability , each dimention of input data got the probability density.
    //And porbability is the multipilication of all the probability density of one row input data.
    for(int i=0;i<label_class.size();i++)
    {
        for(int dim=0;dim<input.size();dim++)
        {
            
            double p=1/pow(2*M_PI*variance[i][dim],0.5)*exp(-pow(input[dim]-mean_value[i][dim],2)/(variance[i][dim]*2));//probability density of input data dimention in each class
            bin1.push_back(p);
        }
        P_Xi_Yk.push_back(bin1);
        bin1.clear();
    }
    // print result
    for(int i=0;i<label_class.size();i++)
    {
        double P_Yk_Xi=1;
        for(int j=0;j<P_Xi_Yk[i].size();j++)
        {
            P_Yk_Xi=P_Yk_Xi*P_Xi_Yk[i][j];
        }
        P_Yk_Xi=P_Yk_Xi*P_Yk[i];
        cout<<"the probability of input data in class label "<<label_class[i]<<"is : "<<P_Yk_Xi<<endl;
    }
}
int main()
{
    // setting Y label string
    vector<string> label_class;
    label_class.push_back("Iris-setosa");
    label_class.push_back("Iris-versicolor");
    label_class.push_back("Iris-virginica");
    // data prehandel
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris_test_label.csv";
    int tr_data_row=150;
    vector<vector<double>> x_data;  //train X data
    vector<string> label;// train Y label
    vector<vector<vector<string>>> my_data = DataPreHandle(train_path,tr_data_row);
    StringToDouble(my_data,label_class,x_data,label);
    vector<vector<double>> x_test_data;//test X data
    vector<string> test_label;// test Y lebel
    vector<vector<vector<string>>> my_test_data=DataPreHandle(test_path,15);
    StringToDouble(my_test_data,label_class,x_test_data,test_label);
    //predict
    GaussianBayesClassifier(x_data,label,label_class,x_test_data[0]);
    GaussianBayesClassifier(x_data,label,label_class,x_test_data[6]);
    GaussianBayesClassifier(x_data,label,label_class,x_test_data[10]);
    cout<<"end";  
}
