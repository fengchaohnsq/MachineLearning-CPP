#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
#include <algorithm>
using namespace std;
vector<int> used_dim;
vector<string> prediction_result;
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
    for (int row =0;row<=data_row;row++)//get row from first line which include the id
    {
        data_bin.push_back(my_data_bin[row][my_data_bin[row].size()-1]);  //get the last data of each row, which is the label data.
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    //set X data
    for (int row =0;row<=data_row;row++)//get row from first line which include the id
    {
        for(int column =0;column<my_data_bin[row].size();column++)
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
This method used for sorting data from small to large
*/
vector<vector<string>> SortByDim(vector<vector<string>> data,int discrete_dim)
{
    for(int data_row=1; data_row<data.size();data_row++)
    {
        for(int data_row2=data_row+1;data_row2<data.size();data_row2++)
        {
            if(atof(data[data_row][discrete_dim].c_str())>atof(data[data_row2][discrete_dim].c_str()))
            {
                vector<string> temp=data[data_row];
                data[data_row]=data[data_row2];
                data[data_row2]=temp;
            }
        }
    }
    return data;
}
/*
this method transpose the matrix.
*/
vector<vector<string>> Matrix_Transpose(vector<vector<string>> data)
{
    vector<string>bin;
    vector<vector<string>> transpose_data;
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
this method is used to get data feature class
*/
vector<vector<string>> GetFeatureClass(vector<vector<string>> data)
{
    vector<vector<string>> x_data=Matrix_Transpose(data);
    vector<vector<string>> feature_class;
    vector<string> feature_bin;
    // get feature class
    for(int i=0;i<x_data.size()-1;i++)//exclude the id and y label
    {
        for(int j=1;j<x_data[i].size();j++)//exclude the label name
        {
            bool flag = true;
            // the first data is included
            if (j==1)
            {
                feature_bin.push_back(x_data[i][j]);
            }
            // comparing all of data class to include the different data
            for(int k=0;k<feature_bin.size();k++)
            {
                if(feature_bin[k]==x_data[i][j])
                {
                    flag = false;
                }
            }
            if(flag)
            {
                feature_bin.push_back(x_data[i][j]);
            }
        }
        feature_class.push_back(feature_bin);
        feature_bin.clear();
    }
    return feature_class;
}
/*
this method calculate the empirical entropy of Y label
*/
double Entropy(vector<vector<string>> data,vector<string> y_label_class)
{
    vector<vector<string>> y_label=data;
    vector<string> label;
    double empirical_entropy=0.0;
    double p=0.0;
    for(int row=1;row<y_label.size();row++)// exclude label name
    {
        label.push_back(y_label[row][data[0].size()-1]);
    }
    for( int class_num=0;class_num<y_label_class.size();class_num++)
    {
        double y_count=count(label.begin(),label.end(),y_label_class[class_num]);
        double label_num=label.size();
        p=y_count/label_num;
        if(p!=0){empirical_entropy=empirical_entropy+(-1*p*log2(p));}
    }
    return empirical_entropy;
}
/*
this method calculate the empirical conditional entropy
*/
double ConditionalEntropy(vector<vector<string>> data,vector<string> y_label_class,int dim)
{
    vector<vector<string>> feature_class=GetFeatureClass(data);  // X feature class of each dimention
    vector<vector<string>> transpose_x_data=Matrix_Transpose(data);
    double conditional_entropy=0.0;
    double H_y_x=0.0;
    // calculate the conditional entropy of the dimention
    for(int dim_class=0;dim_class<feature_class[dim].size();dim_class++)
    {
        // get probability of each class in the dimention
        double feature_class_num=count(transpose_x_data[dim].begin(),transpose_x_data[dim].end(),feature_class[dim][dim_class]);
        double dim_data_size=transpose_x_data[dim].size()-1;
        double p=feature_class_num/dim_data_size;// exclude the label name
        // get entropy of each Y label in the dimention class     
        for(int y_class_num=0;y_class_num<y_label_class.size();y_class_num++)
        {           
            double y_num=0;
            for(int trans_col=1;trans_col<transpose_x_data[dim].size();trans_col++)
            {
             if(transpose_x_data[dim][trans_col]==feature_class[dim][dim_class])
             {
                 int a1=transpose_x_data.size()-1;
                 int a2=trans_col;
                if(transpose_x_data[a1][a2]==y_label_class[y_class_num])
                {
                    y_num++;
                }
             }
            }
            double p_y=y_num/feature_class_num;
            if(p_y!=0)
            {
                H_y_x=H_y_x+(-1*p_y*log2(p_y));
            }
        }
        conditional_entropy=conditional_entropy+p*H_y_x;
        H_y_x=0.0;
    }
    return conditional_entropy;
}
/*
this method calculates the gain information
*/
double GainInfo(vector<vector<string>> data,vector<string> y_label_class,int dim)
{
    double entropy=Entropy(data,y_label_class);
    double conditional_entropy=ConditionalEntropy(data,y_label_class,dim);
    double result = entropy - conditional_entropy;
    return result;
}
/*
this method calculates the gain ratio
*/
double GainRatio(vector<vector<string>> data,vector<string> y_label_class,int dim)
{
    double gain_info=GainInfo(data,y_label_class,dim);
    vector<vector<string>> features=GetFeatureClass(data);
    vector<vector<string>> transpose_x_data=Matrix_Transpose(data);
    double entropy=0.0;
    double p=0.0;
    // calculate selected dimentions' entropy 
    for( int feature_num=0;feature_num<features[dim].size();feature_num++)
    {
        double feature_count=count(transpose_x_data[dim].begin(),transpose_x_data[dim].end(),features[dim][feature_num]);
        double data_num=data.size()-1;
        p=feature_count/data_num;
        if(p!=0){entropy=entropy+(-1*p*log2(p));}
    }
    double result=gain_info/entropy;
    return result;
}
/*
This method will convert continuous data to discrete data.
*/
vector<vector<string>> DataDiscreteByBiPartrition(vector<vector<string>> data,vector<string> y_label_class,int discrete_dim)
{
    //sort data from small to large
    vector<vector<string>> sorted_data= SortByDim(data,discrete_dim);
    //get the partition value array
    vector<double> partition_values;
    double gain_info=0.0;
    double my_partition_value;
    for(int data_row=0;data_row<sorted_data.size()-1;data_row++)// the number of partition value is data_row-1
    {
        double value=(atof(sorted_data[data_row][discrete_dim].c_str())+atof(sorted_data[data_row+1][discrete_dim].c_str()))/2;
        partition_values.push_back(value);
    }
    //find the largest gain info
    for(int i=0;i<partition_values.size();i++)
    {
        vector<vector<string>> data1=data;
        //split data
        for(int data_row=1; data_row<data1.size();data_row++)
        {
            
            if(atof(data[data_row][discrete_dim].c_str())<=partition_values[i])
            {
                data1[data_row][discrete_dim]='1';
            }
            else
            {
                data1[data_row][discrete_dim]='2';
            }
        }
        double my_gain=GainInfo(data1,y_label_class,discrete_dim);
        if(gain_info<my_gain)
        {
            gain_info=GainInfo(data1,y_label_class,discrete_dim);
            my_partition_value=partition_values[i];
        }
    }
    for(int data_row=1; data_row<data.size();data_row++)
    {
        if(atof(data[data_row][discrete_dim].c_str())<=my_partition_value)
        {
            data[data_row][discrete_dim]='1';
        }
        else
        {
            data[data_row][discrete_dim]='2';
        }
    }
    return data;
}
/*multiple tree node*/
struct node
{
    vector<vector<string>> splited_data;
    string data_attribute;  // current node determine attribute which is this dimentions'value.
    string y_label;//this is leaf nodes' classification result. 
    int selected_dimention; // selected feature
    vector<node *> childs; // add child node using 	Node * new_node = new Node(); 		this->childs.push_back(new_node);
    node(vector<vector<string>> data,int dimention=NULL,string attribute="empty",string label="empty")
    {
        splited_data = data;
        selected_dimention= dimention;
        data_attribute=attribute;
        y_label=label;
    }
};
/*
this method returns the dimention of most gain informations,which is used to split data.
*/
int FindDim(vector<vector<string>> data,vector<string> y_label_class,vector<int> used_dim)
{
    double gain_info=0.0;
    double gain_ratio=0.0;
    int selected_dim;
    bool initial=true;// initial flag
    for(int dim=1;dim<data[0].size()-1;dim++)// exclude id column and Y label column
    {
        //if the used_dim included current dim，cause the decision tree used features only ones.
        int flag=count(used_dim.begin(),used_dim.end(),dim);
        if(flag==1)
        {
            continue;
        }
        // first time initial the selected_dim and gain_info
        if(initial)
        {
            selected_dim=dim;
            //gain_info=GainInfo(data,y_label_class,dim);
            gain_ratio=GainRatio(data,y_label_class,dim);
            initial=false;
        }
        // comparing gain information to find the largest gain information
        if(gain_ratio<GainRatio(data,y_label_class,dim))
        {
            selected_dim=dim;
            gain_ratio=GainRatio(data,y_label_class,dim);
            //gain_info=GainInfo(data,y_label_class,dim);
        }
    }
    return selected_dim;
}
/*
this method returns feature value which account for the largest ratio of the dimention.
*/
string FindLargestAttr(vector<vector<string>> data,int selected_dim)
{
    vector<vector<string>> feature_class=GetFeatureClass(data); // already exclude the id feature and y label feature,so the slected dim need to add 1.
    vector<vector<string>> tr_data = Matrix_Transpose(data);
    string attribute;
    int att_num;
    for(int dim_class=0;dim_class<feature_class[selected_dim].size();dim_class++)
    {
        if(dim_class==0)
        {
            att_num=count(tr_data[selected_dim].begin(),tr_data[selected_dim].end(),feature_class[selected_dim][dim_class]);
            attribute=feature_class[selected_dim][dim_class];
        }
        else if(att_num<count(tr_data[selected_dim].begin(),tr_data[selected_dim].end(),feature_class[selected_dim][dim_class]))
        {
            att_num=count(tr_data[selected_dim].begin(),tr_data[selected_dim].end(),feature_class[selected_dim][dim_class]);
            attribute=feature_class[selected_dim][dim_class];
        }
    }
    return attribute;
}
/*
this method is used to split data by selected dimention
*/
vector<vector<vector<string>>> SpliteDataByDim(vector<vector<string>> data, int selected_dim)
{
    vector<vector<string>> tr_data=Matrix_Transpose(data);
    vector<vector<string>> feature_class=GetFeatureClass(data); // already exclude the id feature and y label feature,so the slected dim need to add 1.
    vector<vector<vector<string>>> splited_data;
    vector<vector<string>> bin;
    bin.push_back(data[0]);
    for(int feature_num=0;feature_num<feature_class[selected_dim].size();feature_num++)
    {
        for(int dim_col=1;dim_col<tr_data[selected_dim].size();dim_col++)
        {
            if(tr_data[selected_dim][dim_col]==feature_class[selected_dim][feature_num])
            {
                bin.push_back(data[dim_col]);
            }
        }
        splited_data.push_back(bin);
        bin.clear();
        bin.push_back(data[0]);
    }
    return splited_data;
}
/*
this method returns the largest y_label value of current dimention 
*/
string FindLatgestYlabel(vector<vector<string>> splited_data,vector<string> y_label_class)
{
    int y_num=0;
    string label;
    vector<vector<string>> tr_splited_data=Matrix_Transpose(splited_data);
    int pos=tr_splited_data.size()-1;
    for(int i=0;i<y_label_class.size();i++)
    {
        if(y_num<count(tr_splited_data[pos].begin(),tr_splited_data[pos].end(),y_label_class[i]))
        {
            y_num=count(tr_splited_data[pos].begin(),tr_splited_data[pos].end(),y_label_class[i]);
            label=y_label_class[i];
        }
    }
    return label;
}
/*
this method create decision tree.
*/
void DecisionTree(node *root,vector<string> y_label_class,double thresh_hold,vector<vector<string>> feature_class)
{
    //determine if all instances belong to same class, then return the single node
    for(int y_dim=0;y_dim<y_label_class.size();y_dim++)
    {
        vector<vector<string>> tr_data = Matrix_Transpose(root->splited_data);
        if(count(tr_data[tr_data.size()-1].begin(),tr_data[tr_data.size()-1].end(),y_label_class[y_dim])==tr_data[root->selected_dimention].size()-1)
        {
            root->y_label=y_label_class[y_dim];
            return;
        }
    }
    // all dimention are used except the last dimention
    if(used_dim.size()==(root->splited_data[0].size()-1))
    {
        if(root->selected_dimention==0)
        {
            root->y_label=FindLatgestYlabel(root->splited_data,y_label_class);
            return;
        }
        vector<vector<vector<string>>> splited_data= SpliteDataByDim(root->splited_data,root->selected_dimention);
        for(int feature_num=0;feature_num<splited_data.size();feature_num++)    //spliting data based on the number of dimention classification 
        {
            node *child_node= new node(splited_data[feature_num],NULL,feature_class[root->selected_dimention][feature_num],FindLatgestYlabel(splited_data[feature_num],y_label_class)); // generate new child_node
            root->childs.push_back(child_node);
        }
        return;
    }

    // build tree
    //calculate the largest gain information to find unused dim.
    int selected_dim=FindDim(root->splited_data,y_label_class,used_dim);
    used_dim.push_back(selected_dim);
    root->selected_dimention=selected_dim;
    vector<vector<vector<string>>> splited_data= SpliteDataByDim(root->splited_data,root->selected_dimention);//split data by dimention

    //if the gain information value less than threshhold value, then return the single node.
    if(GainInfo(root->splited_data,y_label_class,root->selected_dimention)<thresh_hold)
    {
        root->y_label=FindLatgestYlabel(root->splited_data,y_label_class);
        return;
    }

    //if data set left one data row only then return node as leaf node
    if(root->splited_data.size()==1)
    {
        root->y_label=FindLatgestYlabel(root->splited_data,y_label_class);
        return;
    }

    // spliting the data as several subsets and made the largest subset as class label.
    for(int feature_num=0;feature_num<splited_data.size();feature_num++)    //spliting data based on the number of dimention classification 
    {
        node *child_node= new node(splited_data[feature_num],0,feature_class[root->selected_dimention][feature_num],"empty"); // generate new child_node
        root->childs.push_back(child_node);
    }

    //recursive treval each child node
    if(root->childs.size()>0)
    {
        for(int i =0;i<root->childs.size();i++)
        {
            DecisionTree(root->childs[i],y_label_class,thresh_hold,feature_class);
        }
    }
}
/*
this method is used to cart the decision tree
*/
double c_loss=0.0;
void CartTree(node *root,double alpha)
{
    if(root->childs.size()==0)//if it didnt have a child node then this is a leaf node
    {
        return;
    }
    for(int i=0;i<root->childs.size();i++)
    {
        if(root->childs.size()==0)
        {
            c_loss=0.0;
            // this is a leaf node
        }
        else
        {
            CartTree(root->childs[i],alpha);
        }
        
    }
}
/*
this method return the result from decision tree prediction.
*/
void DecisionTreePrediction(node *decision_tree,vector<string> data_row)
{ 
    if(decision_tree->childs.size()==0)//if it didnt have a child node then this is a leaf node
    {
        prediction_result.push_back(decision_tree->y_label);
        return;
    }
    for(int i=0;i<decision_tree->childs.size();i++)
    {
        if(decision_tree->childs[i]->data_attribute==data_row[decision_tree->selected_dimention])
        {
            DecisionTreePrediction(decision_tree->childs[i],data_row);
        }
    }

}

int main()
{
    // setting Y label string
    vector<string> y_label_class;
    y_label_class.push_back("Existing Customer");
    y_label_class.push_back("Attrited Customer");
    // setting continuous dimention
    vector<int> continuous_dim;
    continuous_dim.push_back(1);
    continuous_dim.push_back(6);
    continuous_dim.push_back(7);
    //splitting data as X data and Y label
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/bank_data_train.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/bank_data_test.csv";
    int train_data_row=700;
    int test_data_row=300;
    vector<vector<vector<string>>> my_data = DataPreHandle(train_path,train_data_row);
    vector<vector<vector<string>>> my_test_data=DataPreHandle(test_path,test_data_row);
    //discreting the continuous data
    for(int num=0;num<continuous_dim.size();num++)
    {
        my_data[1]=DataDiscreteByBiPartrition(my_data[1],y_label_class,continuous_dim[num]);
        my_test_data[1]=DataDiscreteByBiPartrition(my_test_data[1],y_label_class,continuous_dim[num]);
    }
    //build decision tree
    double thresh_hold=0.0;
    used_dim.push_back(0);
    node *root= new node(my_data[1],0,"empty","empty");
    vector<vector<string>> feature_class=GetFeatureClass(my_data[1]);
    DecisionTree(root,y_label_class,thresh_hold,feature_class);
    //using decision tree to predict
    prediction_result.push_back("prediction result");
    for(int row=1;row<my_test_data[1].size();row++)
    {
        DecisionTreePrediction(root,my_test_data[1][row]);
    }
    for(int row=1;row<my_test_data[1].size();row++)
    {
        // for( int col=0;col<my_test_data[1][row].size();col++)
        // {
        //     string col_name=my_test_data[1][0][col];
        //     string col_value=my_test_data[1][row][col];
        //     cout<<col_name<<":"<<col_value<<"    ";
        // }
        string col_name=my_test_data[1][0][my_test_data[1][row].size()-1];
        string col_value=my_test_data[1][row][my_test_data[1][row].size()-1];
        cout<<col_name<<":"<<col_value<<"    ";
        cout<<"prediction: "<<prediction_result[row]<<endl;
    }
    cout<<"end";
}
