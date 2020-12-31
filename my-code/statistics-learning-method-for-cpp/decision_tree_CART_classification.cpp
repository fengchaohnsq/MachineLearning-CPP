/*
This tree used to generate regression result,which means the result is continuous data. 
If dataset have discrete data then its feature number should not more 3 types.
If the discrete data has more than 3 types of feature, you should find way to decrease its classificaion to<=3;
*/
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
#include <algorithm>
using namespace std;
/*
binary tree node
*/
struct node
{
    vector<vector<string>> splited_data;// current nodes' data.
    string cutting_value="empty";// identify if the data belong to its left or right node. <=cutting value is left, >cutting value is right.
    double gini_index=0.0;//the gini index which used to find the best dimention.
    string prediction="empty";// this is leaf nodes' classification result. 
    int selected_dimention; // selected dimention
    node *left=nullptr; // left node
    node *right=nullptr;// right node
    node *parent=nullptr; // current nodes'parent node
    node(vector<vector<string>> data,node *my_parent,int dimention=0)  //initial the node
    {
        splited_data=data;
        parent=my_parent;
        selected_dimention=dimention;
    }
};
/*
cutting point information
*/
struct cut_point
{
    string cutting_point;
    double cutting_gini_index;
};
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
    for (int row =0;row<=data_row;row++)//get row from first line 
    {
        for(int column =0;column<my_data_bin[row].size();column++)//include the id
        {
            data_bin.push_back(my_data_bin[row][column]);  //get the data of each row.
        }
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    return  my_data;
}
/*
This method used for sorting data from small to big
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
this method is used to get data feature class except the y label column
*/
vector<vector<string>> GetFeatureClass(vector<vector<string>> data)
{
    vector<vector<string>> x_data=Matrix_Transpose(data);
    vector<vector<string>> feature_class;
    vector<string> feature_bin;
    // get feature class
    for(int i=0;i<x_data.size()-1;i++)//exclude the y label
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
this method is used to calculate gini index
*/
double Gini_Index(vector<vector<string>> splited_data,vector<string> y_label_class)
{
    double gini=0;
    for (int label_pos=0; label_pos<y_label_class.size(); label_pos++)
    {
        int count=0;
        for(int i=1;i<splited_data.size();i++)
        {
            if(splited_data[i][splited_data[i].size()-1]==y_label_class[label_pos])
            {
                count++;
            }
        }
        double p_square=pow((double(count)/(splited_data.size()-1)),2);
        gini=gini+p_square;
    }
    gini=1-gini;
    return gini;
}
/*
this method gets variance of continuous data prediction value
*/
double ContinuousDataConditionGini(vector<vector<string>> sorted_data,vector<string> y_label_class,int cut_pos)
{
    double left_gini=0.0;
    double right_gini=0.0;
    //left part
    vector<vector<string>> left_data;
    for (int i = 0; i <= cut_pos; i++)
    {
        left_data.push_back(sorted_data[i]);
    }
    left_gini=Gini_Index(left_data,y_label_class)*(double(left_data.size()-1)/(sorted_data.size()-1));
    //right part
    vector<vector<string>> right_data;
    right_data.push_back(sorted_data[0]);
    for (int i = cut_pos+1; i < sorted_data.size(); i++)
    {
        right_data.push_back(sorted_data[i]);
    }
    right_gini=Gini_Index(right_data,y_label_class)*(double(right_data.size()-1)/(sorted_data.size()-1));
    return left_gini+right_gini;
}
/*
this method will return cutting point and its prediction variance of continuous data.
<= cutting point go left otherwise go right
*/
cut_point ContinuousDataBestCutPoint(vector<vector<string>> data,vector<string> y_label_class,int continuous_dim)
{
    cut_point my_point;
    // sort data from small to big
    vector<vector<string>> sorted_data= SortByDim(data,continuous_dim);
    // find best cutting point
    for(int i=1;i<sorted_data.size();i++)//i represents the data row and cutting position
    {
        if(i==1)
        {
            my_point.cutting_gini_index=ContinuousDataConditionGini(sorted_data,y_label_class,i);
            my_point.cutting_point=sorted_data[i][continuous_dim];
        }
        double current_gini=ContinuousDataConditionGini(sorted_data,y_label_class,i);
        if(my_point.cutting_gini_index>current_gini)
        {
            my_point.cutting_gini_index=current_gini;
            my_point.cutting_point=sorted_data[i][continuous_dim];
        }  
    }
    return my_point;
}
/*
this method gets variance of discrete data prediction value.
the dicerete datas' classification could not more than 3 for this algorithm.
*/
double DiscreteDataConditionGini(vector<vector<string>> data,vector<string> y_label_class,string cutting_feature,int discrete_dim)
{
    double left_gini=0.0;
    double right_gini=0.0;
    vector<vector<string>> left_data;
    left_data.push_back(data[0]);
    vector<vector<string>> right_data;
    right_data.push_back(data[0]);
    for (int i = 1; i < data.size(); i++)
    {
        if(data[i][discrete_dim]==cutting_feature)
        {
            left_data.push_back(data[i]);
        }
        else
        {
            right_data.push_back(data[i]);
        }
    }
    left_gini=Gini_Index(left_data,y_label_class)*(double(left_data.size()-1)/(data.size()-1));
    right_gini=Gini_Index(right_data,y_label_class)*(double(right_data.size()-1)/(data.size()-1));
    return left_gini+right_gini;
}
/*
this method will return cutting point and its prediction variance of discrete data.
equal cutting point go left, otherwise go right.
the dicerete datas' classification could not more than 3 for this algorithm.
*/
cut_point DiscreteDataBestCutPoint(vector<vector<string>> data,vector<vector<string>> feature_class,vector<string> y_label_class,int discrete_dim)
{
    cut_point my_point;
    // find best cutting point
    
    for(int i=0;i<feature_class[discrete_dim].size();i++)
    {
        if(i==0)
        {
            my_point.cutting_gini_index=DiscreteDataConditionGini(data,y_label_class,feature_class[discrete_dim][i],discrete_dim);
            my_point.cutting_point=feature_class[discrete_dim][i];
        }
        double current_gini=DiscreteDataConditionGini(data,y_label_class,feature_class[discrete_dim][i],discrete_dim);
        if(my_point.cutting_gini_index>current_gini)
        {
            my_point.cutting_gini_index=current_gini;
            my_point.cutting_point=feature_class[discrete_dim][i];
        }
    }
    return my_point;
}
/*
this method is used to split data to two parts maximally by selected dimention
*/
vector<vector<vector<string>>> SpliteDataByDim(vector<vector<string>> data,int selected_dim,string cutting_point,bool is_discrete)
{
    vector<vector<vector<string>>> splited_data;
    vector<vector<string>> bin1;
    vector<vector<string>> bin2;
    bin1.push_back(data[0]);
    bin2.push_back(data[0]);
    for(int row=1;row<data.size();row++)
    {
        if(is_discrete)
        {
            if(data[row][selected_dim]==cutting_point)
            {
                bin1.push_back(data[row]);
            }
            else
            {
                bin2.push_back(data[row]);
            }
            
        }
        else
        {
            if(atof(data[row][selected_dim].c_str())<=atof(cutting_point.c_str()))
            {
                bin1.push_back(data[row]);
            }
            else
            {
                bin2.push_back(data[row]);
            }
            
        }  
    }
    splited_data.push_back(bin1);
    splited_data.push_back(bin2);
    return splited_data;
}
/*
this method return the mean value of y label based on the data
*/
string GetPrediction(vector<vector<string>> data,vector<string> y_label_class)
{
    string y_label;
    int my_count=0;
    
    for (int i = 0; i < y_label_class.size(); i++)
    {
        int curr_count=0;
        if(i==0)
        {
            for (int row = 1; row < data.size(); row++)
            {
                if(data[row][data[row].size()-1]==y_label_class[i])
                {
                    curr_count++;
                }
            }
            my_count=curr_count;
            y_label=y_label_class[i];
        }
        else
        {
            for (int row = 1; row < data.size(); row++)
            {
                if(data[row][data[row].size()-1]==y_label_class[i])
                {
                    curr_count++;
                }
            }
            if(curr_count>my_count)
            {
                my_count=curr_count;
                y_label=y_label_class[i];
            }
        }
    }
    return y_label;
}
/*
this recursion method create decision tree.
*/
void DecisionTree(node *root,vector<vector<string>> feature_class,vector<int> &used_dim,vector<int> discrete_dim,vector<string> y_label_class,double gini_threshold)
{
    // all dimention are used except the last dimention
    if(used_dim.size()==(root->splited_data[0].size()-1))
    {
        root->prediction=GetPrediction(root->splited_data,y_label_class);
        return;
    }
    //if datasets' data row less than data number threshhold 3 then return node as leaf node
    if(root->splited_data.size()<8)
    {
        root->prediction=GetPrediction(root->splited_data,y_label_class);
        return;
    }
    // if the variance less than  variance threshhold then return node as leaf node
    if(root->gini_index<gini_threshold)
    {
        root->prediction=GetPrediction(root->splited_data,y_label_class);
        return;
    }
    // build tree
    int selected_dim;
    cut_point best_point;
    bool first_flag=true;
    bool is_discrete;
    // find the best dimention which had the minimum variance.
    for(int dim_pos=1;dim_pos<feature_class.size();dim_pos++)//exclude id dim and feature_class didnt contain y label
    {
        if(count(used_dim.begin(),used_dim.end(),dim_pos)==0)//exclude the used dim
        {
            if(count(discrete_dim.begin(),discrete_dim.end(),dim_pos)>0)// is discrete data
            {
                is_discrete=true;
                if(first_flag)// first time get cutting point
                {
                    best_point=DiscreteDataBestCutPoint(root->splited_data,feature_class,y_label_class,dim_pos);
                    selected_dim=dim_pos;
                    first_flag=false;
                }
                else
                {
                    cut_point new_cut=DiscreteDataBestCutPoint(root->splited_data,feature_class,y_label_class,dim_pos);
                    if(new_cut.cutting_gini_index<best_point.cutting_gini_index)// find the best dimention
                    {
                        best_point=new_cut;
                        selected_dim=dim_pos;
                    }
                }
            }
            else  // is continuous data
            {
                is_discrete=false;
                if(first_flag)// first time get cutting point
                {
                    best_point=ContinuousDataBestCutPoint(root->splited_data,y_label_class,dim_pos);
                    selected_dim=dim_pos;
                    first_flag=false;
                }
                else
                {
                    cut_point new_cut=ContinuousDataBestCutPoint(root->splited_data,y_label_class,dim_pos);
                    if(new_cut.cutting_gini_index<best_point.cutting_gini_index)// find the best dimention
                    {
                        best_point=new_cut;
                        selected_dim=dim_pos;
                    }
                }
            }
        }
    }
    root->cutting_value=best_point.cutting_point;
    root->gini_index=best_point.cutting_gini_index;
    root->selected_dimention=selected_dim;
    used_dim.push_back(selected_dim);
    // spliting data by selected dimention
    vector<vector<vector<string>>> splited_data= SpliteDataByDim(root->splited_data,root->selected_dimention,root->cutting_value,is_discrete);
    // setting left node and right node
    node *left_node=new node(splited_data[0],root,0);
    node *right_node=new node(splited_data[1],root,0);
    root->left=left_node;
    root->right=right_node;
    DecisionTree(root->left,feature_class,used_dim,discrete_dim,y_label_class,gini_threshold);
    DecisionTree(root->right,feature_class,used_dim,discrete_dim,y_label_class,gini_threshold);
}
/*
this method gets all unduplicate leafs which had same parent node.
recursion method has to pass the address to do the recusion call.
*/
void GetLeafNodes(node* root,vector<node*> &my_leafs)
{
    if(root->left!=nullptr)
    {
        GetLeafNodes(root->left,my_leafs);
    }
    if(root->right!=nullptr)
    {
        GetLeafNodes(root->right,my_leafs);
    }
    if(root->left==nullptr&&root->left==nullptr)
    {
        my_leafs.push_back(root);
    }
}
/*
this method calculates the alpha value for cutting tree.
root_t represent the inside node which will be cutted.
*/
double CalAlphaLoss(node* root_t)
{
    double loss_cut_before=0.0;
    double loss_cut_after=0.0; 
    double alpha=0.0;
    vector<node*> my_leafs;
    GetLeafNodes(root_t,my_leafs);
    loss_cut_after=root_t->gini_index;
    int node_num=my_leafs.size();
    for(int leaf_num=0;leaf_num<node_num;leaf_num++)
    {
        loss_cut_after=loss_cut_after+my_leafs[leaf_num]->gini_index;
    }
    alpha=(loss_cut_after-loss_cut_before)/node_num;
    return alpha;
}
/*
this recursion method is used to trim tree
the tree is pointer type struct which passed a address，so it need not to declare in the public region.
*/
void TrimTree(node* tree,node *cut_node,vector<string> y_label_class)
{
    if((tree->selected_dimention==cut_node->parent->selected_dimention) && (tree->cutting_value==cut_node->parent->cutting_value))
    {
        tree->left=nullptr;
        tree->right=nullptr;
        tree->prediction=GetPrediction(tree->splited_data,y_label_class);
        return;
    }
    if(tree->left!=nullptr){TrimTree(tree->left,cut_node,y_label_class);}
    if(tree->right!=nullptr){TrimTree(tree->right,cut_node,y_label_class);}
}
/*
this method copies a binary tree 
*/
void CopyTree(node* root,node* copy_tree)
{
    // copy value
    if(root->parent==nullptr)// copy the root node
    {
        copy_tree->parent=nullptr;
        copy_tree->cutting_value=root->cutting_value;
        copy_tree->prediction=root->prediction;
        copy_tree->selected_dimention=root->selected_dimention;
        copy_tree->gini_index=root->gini_index;
        copy_tree->splited_data=root->splited_data;
    }
    else// copy inside node and leaf nodes
    {
        copy_tree->cutting_value=root->cutting_value;
        copy_tree->prediction=root->prediction;
        copy_tree->selected_dimention=root->selected_dimention;
        copy_tree->gini_index=root->gini_index;
        copy_tree->splited_data=root->splited_data;
    }
    // address type data copy
    if(root->left!=nullptr)
    {
        //initial left node
        node* copy_left=new node(root->splited_data,nullptr,0);// initial node, root->splited_data is not correct data;
        copy_left->parent=copy_tree; 
        copy_tree->left=copy_left;
        CopyTree(root->left,copy_tree->left);
    }
    if(root->right!=nullptr)
    {
        // initial right node
        node* copy_right=new node(root->splited_data,nullptr,0);// initial node, root->splited_data is not correct data;
        copy_right->parent=copy_tree;
        copy_tree->right=copy_right;
        CopyTree(root->right,copy_tree->right);
    }
}
/*
this method find the best subtree sequence
*/
void FindSubtreeSequence(node *root,vector<node*> &child_tree_sequence,vector<string> y_label_class)
{
    //get initial leafs 
    vector<node*> leafs;
    GetLeafNodes(root,leafs);
    double minimal_alpha;
    node* cut_node;
    //find best cut node;
    for (int leaf_num=0;leaf_num<leafs.size();leaf_num++)
    {
        if (leaf_num==0)
        {
            minimal_alpha=CalAlphaLoss(leafs[leaf_num]->parent);
            cut_node=leafs[leaf_num];
        }
        double alpha=CalAlphaLoss(leafs[leaf_num]->parent);
        if (minimal_alpha<alpha)
        {
            minimal_alpha=alpha;
            cut_node=leafs[leaf_num];
        }
    }
    
    node* subtree = new node(root->splited_data,nullptr,0);
    CopyTree(root,subtree);
    TrimTree(subtree,cut_node,y_label_class);
    //if the father node is root node
    if(subtree->left==nullptr && subtree->right==nullptr)
    {
        return;
    }
    child_tree_sequence.push_back(subtree);
    FindSubtreeSequence(subtree,child_tree_sequence,y_label_class);
}
/*
this recursion method return the result from decision tree prediction which is a vector type of data.
*/
void DecisionTreePrediction(node *decision_tree,vector<string> data_row,vector<string> &prediction,vector<int> discrete_dim)
{ 
    if(decision_tree->left==nullptr&&decision_tree->right==nullptr)//if it didnt have a child node then this is a leaf node
    {
        prediction.push_back(decision_tree->prediction);
        return;
    }
    //for discrete data
    if(count(discrete_dim.begin(),discrete_dim.end(),decision_tree->selected_dimention)==1)
    {
        //go left
        if(decision_tree->cutting_value==data_row[decision_tree->selected_dimention])
        {
            prediction.push_back(decision_tree->prediction);
            DecisionTreePrediction(decision_tree->left,data_row,prediction,discrete_dim);
        }
        else//go right
        {
            prediction.push_back(decision_tree->prediction);
            DecisionTreePrediction(decision_tree->right,data_row,prediction,discrete_dim);
        }
        
    }
    else//for continuous data
    {
        //go left
        if(atof(data_row[decision_tree->selected_dimention].c_str())<=atof(decision_tree->cutting_value.c_str()))
        {
            prediction.push_back(decision_tree->prediction);
            DecisionTreePrediction(decision_tree->left,data_row,prediction,discrete_dim);
        }
        else//go right
        {
            prediction.push_back(decision_tree->prediction);
            DecisionTreePrediction(decision_tree->right,data_row,prediction,discrete_dim);
        }
    }
}
/*
this method is used to cross validation for those subtree sequence and selecting the best subtree.
*/
node* CrossValidation(vector<node*> subtree_sequence,vector<vector<string>> data,vector<int> discrete_dim)
{
    node* subtree;
    double err;
    for (int i=0;i<subtree_sequence.size();i++)
    {
        vector<string> prediction;
        int err_count=0;
        for (int row=1;row<data.size();row++)
        {
            DecisionTreePrediction(subtree_sequence[i],data[row],prediction,discrete_dim);
        }
        for (int row=1;row<data.size();row++)
        {
            if(data[row][data[row].size()-1]!=prediction[row]){err_count++;}
        }
        double err_pred=err_count/(data.size()-1);
        if(i==0)
        {
            err=err_pred;
            subtree=subtree_sequence[i];
        }
        if(err<err_pred)
        {
            err=err_pred;
            subtree=subtree_sequence[i];
        }
    }
    return subtree;
}
int main()
{
    // setting Y label string
    vector<string> y_label_class;
    y_label_class.push_back("Existing Customer");
    y_label_class.push_back("Attrited Customer");
    // setting continuous dimention
    vector<int> discrete_dim;
    discrete_dim.push_back(2);
    discrete_dim.push_back(3);
    discrete_dim.push_back(4);
    discrete_dim.push_back(5);
    //splitting data as X data and Y label
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/bank_data_train.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/bank_data_test.csv";
    int train_data_row=700;
    int test_data_row=300;
    vector<vector<vector<string>>> my_data = DataPreHandle(train_path,train_data_row);
    vector<vector<vector<string>>> my_test_data=DataPreHandle(test_path,test_data_row);
    //build decision tree
    double gini_thresh_hold=0.0;// this can be used to control the structure of original tree, to prevent it generates unbalanced tree
    vector<int> used_dim;
    used_dim.push_back(0);
    //initialing the root node
    node *root= new node(my_data[1],nullptr,0);
    vector<vector<string>> feature_class=GetFeatureClass(my_data[1]);
    //training decision tree
    DecisionTree(root,feature_class,used_dim,discrete_dim,y_label_class,gini_thresh_hold);
    //generate subtree sequence
    vector<node*> subtree_sequence;
    FindSubtreeSequence(root,subtree_sequence,y_label_class);
    //cross validation find the optimized subtree
    node*best_subtree=CrossValidation(subtree_sequence,my_test_data[1],discrete_dim);
    //using decision tree to predict    
    return 0;
}
