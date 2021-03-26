/*
This project is created by Basant Elhussein and Mariam Fawzy as part of the Digital Design I Course.
March 26, 2021.
Copyrgiht © 2021 Basant and Mariam. All rights are reserved.
*/

#include<bits/stdc++.h>
#define isOn(x,y) (((x)>>(y))&1)
using namespace std;

int num;
vector<int> minterms;

//Function to map the minterm to its location in K-map
pair<int, int> get_Row_Col(int minterm, int num)
{
	int x, y;
	if (num == 2) {
		x = isOn(minterm, 0);
		y = isOn(minterm, 1);
	}
	if (num == 3) {
		x = isOn(minterm, 0);
		if (isOn(minterm, 2)) y = 2 + !isOn(minterm, 1);
		else y = isOn(minterm, 1);
	}
	else if (num == 4) {
		if (isOn(minterm, 1)) x = 2 + !isOn(minterm, 0);
		else x = isOn(minterm, 0);
		if (isOn(minterm, 3)) y = 2 + !isOn(minterm, 2);
		else y = isOn(minterm, 2);
	}
	pair<int, int> sol = { x,y };
	return sol;
}


void buildKmap(){
	pair<int, int> sz[5];
	sz[2] = { 2,2 };
	sz[3] = { 2,4 };
	sz[4] = { 4,4 };
	vector<vector<int>> kmap(sz[num].first, vector<int>(sz[num].second, 0));
	for (auto& i : minterms) {
		pair<int, int> coor = get_Row_Col(i, num);
		kmap[coor.first][coor.second] = 1;
	}
    //Output Result K-map
	cout << "Kmap:\n";
	for (int i = 0; i < kmap.size(); i++) {
		for (int j = 0; j < kmap[i].size(); j++)
			cout << kmap[i][j] << " ";
		cout << endl;
	}
    cout<< "\n-------------\n";
}

unordered_map<int, string> binaryRep;

void getBinaryRep() {
	for (int minTerm = 0; minTerm < pow(2, num); minTerm++) {
		string str;
		for (int bit = num - 1; bit >= 0; bit--)
			if (isOn(minTerm, bit)) str += "1";
			else str += "0";
		binaryRep[minTerm] = str;
	}
}


vector<string> PI;
vector<int> essential, nonessential, chosenNonessential, simpleFunction;
vector<vector<bool>> tablePI;

void getPrimeImplicants() {
	int n = minterms.size(); 
	int vis[(int)pow(2, num)] = {}; 

	for (int common = n; common > 0; common--) {
		if (common != pow(2, (int)log2(common))) continue;

		vector<bool> pick(n, 0);
		fill(pick.end() - common, pick.end(), true);

		do {
			int cntVis = 0;
			vector<int> group;
			for (int i = 0; i < n; ++i) {
				if (pick[i]) {
					group.push_back(minterms[i]);
					if (vis[minterms[i]] > common) cntVis++;
				}
			}
			//If they were all found in larger PI
			if (cntVis == common) continue;

			int matched[4] = {};
			string cur = binaryRep[group[0]];

			// Try matching this minterm with common-1 minterms
			for (int j = 1; j < group.size(); j++) {
				for (int b = 0; b < num; b++) {
					if (cur[b] == binaryRep[group[j]][b]) matched[b]++;
				}
			}

			//Find out if it worked!
			vector<int> chosen;
			for (int ch = 0; ch < num; ch++) if (matched[ch] == common - 1) chosen.push_back(ch);

			if (chosen.size() < num-log2(common))
				continue; //failed to find log(common) fixed bits!


			//Vis the minterms of the PI
            for(auto j:group) if(vis[j]<common)vis[j] = common;
			string ans = "";
			for (int idx = 0; idx < cur.size(); idx++) {
				if (find(chosen.begin(), chosen.end(), idx) != chosen.end()) ans += cur[idx];
				else ans += "*";
			}
			PI.push_back(ans);
			vector<bool> temp(pow(2,num), 0);
			for(auto g:group){
                temp[g] = 1;
			}
			tablePI.push_back(temp);
		} while (next_permutation(pick.begin(), pick.end()));
	}
	cout<< "Prime Implicants:\n";
	for (auto i : PI) cout << i << endl;
	cout<< "\n---------------\n";
}

void simplifyFunction()
{
    int n = minterms.size();
    for(int i=0; i<pow(2, num); i++){
        int cnt = 0,r;
        for(int j=0; j<tablePI.size(); j++) if(tablePI[j][i]==1) cnt++, r=j;
        if(cnt==1) essential.push_back(r);
    }

    vector<int>::iterator itr;
    itr = unique(essential.begin(), essential.end());
    essential.resize(distance(essential.begin(), itr));

    for(int i=0; i<PI.size(); i++){
        if(find(essential.begin(), essential.end(), i)==essential.end()) nonessential.push_back(i);
    }

    int vis[(int)pow(2, num)] = {};
    for(int i=0; i<essential.size(); i++){
        int es = essential[i];
        for(int j=0; j<tablePI[es].size(); j++)
            if(tablePI[es][j]) vis[j] = 1;
    }

    vector<bool> pick(nonessential.size(), 0);
    bool done = 0;
    for(int select=0; select <= nonessential.size() && !done; select++){
        fill(pick.end() - select, pick.end(), true);
        do{
            vector<int> group;
            for(int i=0; i<nonessential.size(); i++){
                if(pick[i]){
                    int non = nonessential[i];
                    group.push_back(non);
                    for(int j=0; j<tablePI[non].size(); j++)
                        if(tablePI[non][j] && !vis[j]) vis[j] = 2;
                }
            }
            bool ok = 1;
            for(int i=0; i<n; i++){
                if(!vis[minterms[i]]) ok = 0;
            }
            if(ok) {
                chosenNonessential = group;
                done = 1;
                break;
            }
            else
            for(int i=0; i<group.size(); i++){
                int non = group[i];
                for(int j=0; j<tablePI[non].size(); j++)
                    if(tablePI[non][j] && vis[j]==2) vis[j] = 0;
            }
        }while(next_permutation(pick.begin(), pick.end()));
    }
    simpleFunction = essential;
    for(auto i:chosenNonessential) simpleFunction.push_back(i);
    cout<< "The final function:\n";
    for(int i=0; i<simpleFunction.size();i++){
        int cur = simpleFunction[i];
        string str;
        for(int j=0; j<num; j++){
            if(PI[cur][j]=='*') continue;
            if(PI[cur][j]=='0') str += (j + 'A'), str += '\'';
            else str += (j + 'A');
        }
        cout<< str << " " << "+ "[i==simpleFunction.size()-1]<<" ";
    }
    if(minterms.size()==(int)pow(2, num)) cout<< 1<<endl;
    else if(!minterms.size()) cout<< 0 << endl;
    cout<< "\n\nCongrats!";
}


int main()
{
	//User Interface
	string str;
	cout << "Please Enter the Number of Variables: ";
	cin >> num;

	//Validation of number of variables
	while(num<2 || num>4){
        cout<< "Please enter a number between 2 and 4\n";
        cin>>num;
	}

	cout << "Please Enter the Minterms in one line: ";
	cin.ignore();
	bool ok = 0;

	//Validation of minterms
	int temp;
	while (!ok && getline(cin, str)) {
		stringstream ss(str);
		ok = 1;
		minterms.clear();
		while (ss >> temp) {
			if (temp < 0 || temp >= (1 << num)) {
				cout << "Invalid input!\nMinterms should be greater than or equal to zero and less than 2^(number of variables).\nTry Again!\n";
				ok = 0;
			}
			minterms.push_back(temp);
		}
	}

	getBinaryRep();
    buildKmap();
	getPrimeImplicants();
	simplifyFunction();
}
