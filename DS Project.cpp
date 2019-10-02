#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <windows.h>
#include <conio.h>

using namespace std;

class suffix_node{
	
	friend class suffix_tree;
    suffix_node *branch[256];
    suffix_node *suffixLink;
    int start;
    int *end;
    int index;
    
    public:
    	suffix_node(){			
			start=0;
			end=0;
			index=0;
		}
};

class suffix_tree{
	
	private:
    string s;
	suffix_node  *root; //Pointer to root node
	suffix_node  *previous_new_node;
	
	//active point
	suffix_node  *active_node;
	int active_edge;
	int active_length;

	int suffix_count;
	int leaf_end;
	int *root_end;
	int *split_end;
	int instring_length; //Length of input string
	int firststring_length; //Size of 1st string
	
	//Destructor
	void deallocate_tree(suffix_node *node){
		if (node == NULL)
		    return;
		
		int i;
		
		for(i = 0; i < 256; i++){
		    if(node->branch[i] != NULL){
		        deallocate_tree(node->branch[i]);
		    }
		}
		
		if(node->index == -1)
		    delete (node->end);
		    
		delete (node);
	}
	    
	public:
	suffix_tree(){
		root = NULL;
		previous_new_node = NULL;
		active_node = NULL;
		active_edge = -1;
		active_length = 0;
		suffix_count = 0;
		leaf_end = -1;
		root_end = NULL;
		split_end = NULL;
		instring_length = -1; 
		firststring_length = 0; 
	}
	
	void setstring(string myString){s=myString;}
	void setsize_of_firststring(int num){firststring_length=num;}
	suffix_node* getroot(){return root;}
	
	suffix_node* newNode(int start, int *end){
		suffix_node *node=new suffix_node();
		int i;
		for (i = 0; i < 256; i++)
		      node->branch[i] = NULL;
		
		node->suffixLink = root;
		node->start = start;
		node->end = end;
		
		node->index = -1;
		return node;
	}
	
	int edge_length(suffix_node *node){
		if(node == root)
			return 0;
		return *(node->end) - (node->start) + 1;
	}
	
	int walkDown(suffix_node *currNode){
		if (active_length >= edge_length(currNode)){
		    active_edge += edge_length(currNode);
		    active_length -= edge_length(currNode);
		    active_node = currNode;
		    return 1;
		}
		return 0;
	}
	
	void start_phase(int pos){
		
		leaf_end = pos;
		suffix_count++;
		
		previous_new_node = NULL;
		
		while(suffix_count > 0) {
		
		    if (active_length == 0)
		        active_edge = pos; 
		
		    if (active_node->branch[s[active_edge]] == NULL){
		        active_node->branch[s[active_edge]] =newNode(pos, &leaf_end);
		        if (previous_new_node != NULL){
		            previous_new_node->suffixLink = active_node;
		            previous_new_node = NULL;
		        }
		    }
	
		    else{
		        suffix_node  *next = active_node->branch[s[active_edge]];
		        if (walkDown(next)){
		            continue;
		        }
		    
		        if (s[next->start + active_length] == s[pos]){
					if(previous_new_node != NULL && active_node != root){
						previous_new_node->suffixLink = active_node;
						previous_new_node = NULL;
					}
					
		            active_length++;
		            break;
		        }
		
		        split_end = new int;
		        *split_end = next->start + active_length - 1;
		
		        suffix_node  *split = newNode(next->start, split_end);
		        active_node->branch[s[active_edge]] = split;
		
		        split->branch[s[pos]] = newNode(pos, &leaf_end);
		        next->start += active_length;
		        split->branch[s[next->start]] = next;
		
		        if (previous_new_node != NULL){
		            previous_new_node->suffixLink = split;
		        }
		
		        previous_new_node = split;
		    }
		
		    suffix_count--;
		    if (active_node == root && active_length > 0){
		        active_length--;
		        active_edge = pos - suffix_count + 1;
		    }
		    else if (active_node != root){
		        active_node = active_node->suffixLink;
		    }
		}
	} 
	
	void deallocate(suffix_node *node){
		if (node == NULL)
		    return;
		
		int i;
		
		for (i = 0; i < 256; i++){
		    if (node->branch[i] != NULL){
		        deallocate_tree(node->branch[i]);
		    }
		}
		
		if (node->index == -1)
		    delete (node->end);
		    
		delete (node);
	}
	
	void create_tree()
	{
		instring_length = s.length();
		root_end = new int;
		*root_end = - 1;
		
		root = newNode(-1, root_end);
		
		active_node = root; 
		
		for(int i=0; i<instring_length; i++)
		    start_phase(i);
		    
		setSuffixIndex(root, 0);
	}
	
	void setSuffixIndex(suffix_node *node, int label_height)
	{
		if(node == NULL)  return;

		int leaf = 1;
		int i;
		for(i = 0; i < 256; i++){
		    if (node->branch[i] != NULL){
		        leaf = 0;
		        setSuffixIndex(node->branch[i],label_height+edge_length(node->branch[i]));
		    }
		}
		if(leaf == 1)
		{
			for(i= node->start; i<= *(node->end); i++){
				if(s[i] == '#')
				{
					node->end = new int;
					*(node->end) = i;
				}
			}
		    node->index = instring_length - label_height;
		}
	}

	int traverse_tree(suffix_node *node, int label_height, int* max_height, int* substring_index){
		if(node == NULL){
			return 0;
		}
		
		int checker = -1;
		
		if(node->index < 0){
			for (int i = 0; i < 256; i++){
				if(node->branch[i] != NULL){
					checker = traverse_tree(node->branch[i],label_height+edge_length(node->branch[i]),max_height,substring_index);
					
					if(node->index == -1)
					node->index = checker;
					
					else if((node->index == -2 && checker == -3) || (node->index == -3 && checker == -2) || node->index == -4){
						node->index = -4;
						
						if(*max_height < label_height){
							*max_height = label_height;
							*substring_index = *(node->end) - label_height + 1;
						}
					}
				}
			}
		}
		else if(node->index > -1 && node->index < firststring_length)//suffix of X
			return -2;//Mark node as X
		
		else if(node->index >= firststring_length)//suffix of Y
			return -3;//Mark node as Y
		
		return node->index;
	}
	
    int longest_common_string(){
		
		int max_height = 0,substring_index = 0,k = 0;
		traverse_tree(root, 0, &max_height, &substring_index);
		if(max_height>50){
			cout<<"\n\nIndex Location: "<<substring_index<<" - "<< max_height+substring_index;
		}
		
		return max_height;
		
		cout<<"\n";
	}
		
};

class Protein{
	private:
	string protein_string; //unknown protein string
	int size;
	suffix_tree stree;
	bool domain_checker[15]; //15 is the number of domains used
		
	public:
		Protein(string s=""){
			protein_string=s;
			size=s.length();
			stree.setsize_of_firststring(size);
			for(int i=0;i<15;i++){
				domain_checker[i]=false;
			}
		}
	    
	    void setprotein(fstream &obj){
	    	string s;
			obj>>s;
	    	
	    	while(!obj.eof()){
			obj>>s;
		    protein_string.append(s);
			}
	    	
	    	size=protein_string.length();
	    	stree.setsize_of_firststring(size);
		}	
		
		int max_of_domain(fstream &obj1){
			string s,s1,s2;
			char c;
			int max=0;
			
			obj1>>s;
			s2=s;
			while(2){
				obj1>>s;
				c=s[0];
				
				if(obj1.eof())
					break;
				
				else if(c=='>'){
				
				    int x=protein_comparator(s1);
				    
					if(x>max){
					max=x;
					}
						
				    if(x>50){
				    cout<<endl;
				    cout<<s2;
				    cout<<"\tHighest similarity match: ";
				    cout<<x<<endl;
				    }
				    
					s2=s;
					s1="";
					s="";
					continue;
				}
				else s1.append(s);
			}
			
			return max;
		}
		
		int checkSh2domain(){
			
			cout<<"\n\nAnalyzing SH2 Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("SH2.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkPhdomain(){
			
			cout<<"\n\nAnalyzing PH Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("PH.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkRhogefdomain(){
			
			cout<<"\n\nAnalyzing RhoGEF Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("RhoGEF.txt",ios::in);
			
			return max_of_domain(obj1);
		
		}
		
		int checkSh3domain(){
			
			cout<<"\n\nAnalyzing SH3 Domain: ";
            cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("SH3.txt",ios::in);
			
			return max_of_domain(obj1);
		
		}
		
		int checkRGSdomain(){
		
			cout<<"\n\nAnalyzing RGS Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("RGS.txt",ios::in);
			
			return max_of_domain(obj1);
			
			
		}
		
		int checkC1domain(){
		
			cout<<"\n\nAnalyzing C1 Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("C1.txt",ios::in);
			
			return max_of_domain(obj1);
		
		}
		
		int checkChdomain(){
			
			cout<<"\n\nAnalyzing CH Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("CH.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkBardomain(){
			
			cout<<"\n\nAnalyzing BAR Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("BAR.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkBahdomain(){
			
			cout<<"\n\nAnalyzing BAH Domain: ";
			cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("BAH.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkArfgapdomain(){
			
			cout<<"\n\nAnalyzing ArfGap Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("ArfGAP.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkAnkdomain(){
		
			cout<<"\n\nAnalyzing ANK_2 Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("ANK_2.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkHistonedomain(){
			
			cout<<"\n\nAnalyzing Histone Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("Histone.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkRasGapdomain(){
			
			cout<<"\n\nAnalyzing RasGap Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("RASGAP.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkRASGEFdomain(){
			
			cout<<"\n\nAnalyzing RASGef Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("RASGEF.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		int checkRAdomain(){
			
			cout<<"\n\nAnalyzing RA Domain: ";
		    cout<<"\nStructures present: "<<endl;
			fstream obj1;
			obj1.open("RA.txt",ios::in);
			
			return max_of_domain(obj1);
			
		}
		
		void domain_analyzer(){
			
			if(checkSh2domain()>50){ //1.
			cout<<"\n\n\t-----------------SH2 Domain Present-----------------\n\n";
		    domain_checker[0]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			
			if(checkPhdomain()>50){ //2.
			cout<<"\n\n\t-----------------PH Domain Present-----------------\n\n";
			domain_checker[1]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkRhogefdomain()>50){ //3.
			cout<<"\n\n\t-----------------RhoGEF Domain Present-----------------\n\n";
			domain_checker[2]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkSh3domain()>50){ //4.
			cout<<"\n\n\t-----------------SH3 Domain Present-----------------\n\n";
			domain_checker[3]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkRGSdomain()>50){ //5.
			cout<<"\n\n\t-----------------RGS Domain Present-----------------\n\n";
			domain_checker[4]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkBahdomain()>50){ //6.
			cout<<"\n\n\t-----------------BAH Domain Present-----------------\n\n";
			domain_checker[5]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkC1domain()>50){ //7.
			cout<<"\n\n\t-----------------C1 Domain Present-----------------\n\n";
			domain_checker[6]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkChdomain()>50){ //8.
			cout<<"\n\n\t-----------------CH Domain Present-----------------\n\n";
			domain_checker[7]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkArfgapdomain()>50){ //9.
			cout<<"\n\n\t-----------------ArfGAP Domain Present-----------------\n\n";
			domain_checker[8]=true;
			}
		    else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
		    
		    if(checkAnkdomain()>50){ //10.
			cout<<"\n\n\t-----------------ANK_2 Domain Present-----------------\n\n";
			domain_checker[9]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkRASGEFdomain()>50){ //11.
			cout<<"\n\n\t-----------------RASGef Domain Present-----------------\n\n";
			domain_checker[10]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
				
			if(checkHistonedomain()>50){ //12.
			cout<<"\n\n\t-----------------Histone Domain Present-----------------\n\n";
			domain_checker[11]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkRasGapdomain()>50){ //13.
			cout<<"\n\n\t-----------------RASGAP Domain Present";
			domain_checker[12]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
				
		    if(checkRAdomain()>50){ //14.
			cout<<"\n\n\t-----------------RA Domain Present";
			domain_checker[13]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
			
			if(checkBardomain()>50){ //15.
			cout<<"\n\n\t-----------------BAR Domain Present";
			domain_checker[14]=true;
			}
			else cout<<"\txxxxxxxxxxxxxxxxx Not present xxxxxxxxxxxxxx\n\n";
				
		}
		
		void protein_function_predictor(){
			
			int func_count=0;
			
			for(int i=0;i<15;i++){
				if(domain_checker[i]){
					fstream obj;
					obj.open("Functions.txt",ios::in);
					while(!obj.eof()){
						string s;
						obj>>s;
						int a = i+1;
						stringstream ss;
						ss << a;
						string str = ss.str();
						if(s==str){
							obj.ignore();
							getline(obj,s);
							cout<<"\n\nFunction "<<++func_count<<": \n";
							cout<<s;
						}
					}
				}
				else continue;
			}
			
		}
		
		int protein_comparator(string s1){
			stree.setstring(protein_string+"#"+s1+"$");
			stree.create_tree(); 
			int x=stree.longest_common_string();
			stree.deallocate(stree.getroot());
			
			return x;
		}
			
		string getstring(){return protein_string;}
		int getsize(){return size;}
		
};

int main(){
	    
	    cout<<"\t\tPROJECT TITLE: DOMAIN SEARCHING IN PROTEINS USING SUFFIX TREES\n\n";
	    cout<<"\nMade by: \n1.Murad Popattia 17k-3722 \n2.Siddahant Kumar 17k-2372 \n3.Ali Akbar 17k-3871";
	    
		fstream obj;
		obj.open("Protein#1.txt",ios::in);
		Protein p;
		p.setprotein(obj);
		
		p.domain_analyzer();
		
		cout<<"\n\n\t\t\t\t|PREDICTED FUNCTIONALITIES|\n\n\n";
		p.protein_function_predictor();
}
