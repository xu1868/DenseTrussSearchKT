#include "mynet.h"

PMyNet TMyNet::LoadTxt(const TStr& FNm, const TStr& MovieFNm, TIntV& n1, TIntV& n2, TIntV& n3, TIntV& n4){
	TStrV ColV;
	char line[2024];
	TIntV nv1, nv2, nv3, nv4;
		std::string key1 = "night";
		std::string key2 = "story";
		std::string key3 = "king";
		std::string key4 = "house";
		FILE *fin = open_file(MovieFNm.CStr(), "r");
		//fgets(line, 2024, fin);
		while(!feof(fin)){
			memset(line, 0, 2024);
			fgets(line, 2024, fin);
			if(strlen(line) == 0) break;
			TStr(line).SplitOnAllCh(':', ColV, true);
			//printf("%d ", ColV.Len());
			int id = ColV[0].GetTrunc().GetInt();
			TStr title;
			title = ColV[1].GetTrunc().GetLc();
			//TStr tit = ColV[1].GetLc();
			std::string ti = title.CStr();
			//printf("%d ", id);
			if(ti.find(key1) < ti.length()){
				nv1.AddUnique(id);
				//printf("1 %d \n", id);
			}
			if(ti.find(key2) < ti.length()){
				nv2.AddUnique(id);
			}
			if(ti.find(key3) < ti.length()){
				nv3.AddUnique(id);
			}
			if(ti.find(key4) < ti.length()){
				nv4.AddUnique(id);
			}
		}
		printf("%d %d %d %d\n", nv1.Len(), nv2.Len(), nv3.Len(), nv4.Len());

	PMyNet net = TMyNet::New();
	int NLines=0, DupEdge=0, UserNId, MovieNId, rate;
	TIntH UserNIdH;
	TIntH MovieNIdH;
	TIntH ratingh;
	//TIntV n1, n2, n3, n4;
	memset(line, 0, 2024);
	FILE *F = fopen(FNm.CStr(), "rt");
	//fgets(line, 2024, F);
	while( !feof(F)){
		memset(line, 0, 2024);
		fgets(line, 2024, F);
		if(strlen(line) == 0) break;
		TStr(line).SplitOnAllCh(':', ColV, true);
		const int UserStrId = ColV[0].GetTrunc().GetInt();
		const int MovieStrId = ColV[1].GetTrunc().GetInt();
		const int rating = ColV[2].GetTrunc().GetInt();
	    //create nodes
		if (UserNIdH.IsKey(UserStrId)){
			UserNId = UserNIdH.GetDat(UserStrId);
		}
		else {
			UserNId = net->AddNode(-1);
			UserNIdH.AddDat(UserStrId, UserNId);
		}

		if (MovieNIdH.IsKey(MovieStrId)){
			MovieNId = MovieNIdH.GetDat(MovieStrId);
		}
		else {
			MovieNId = net->AddNode(-1);
			MovieNIdH.AddDat(MovieStrId, MovieNId);
		}
		if(ratingh.IsKey(rating)){
			rate = ratingh.GetDat(rating);
		}
		else{
			rate = net->AddNode(-1);
			ratingh.AddDat(rating, rate);
			printf("%d %d\n", rating, rate);
		}

		if(nv1.IsIn(MovieStrId)){
			n1.AddUnique(MovieNId);
			n1.AddUnique(UserNId);
			n1.AddUnique(rate);
		}
		if(nv2.IsIn(MovieStrId)){
			n2.AddUnique(MovieNId);
			n2.AddUnique(UserNId);
			n2.AddUnique(rate);
		}
		if(nv3.IsIn(MovieStrId)){
			n3.AddUnique(MovieNId);
			n3.AddUnique(UserNId);
			n3.AddUnique(rating);
		}
		if(nv4.IsIn(MovieStrId)){
			n4.AddUnique(MovieNId);
			n4.AddUnique(UserNId);
			n4.AddUnique(rating);
		}

		if (!net->IsEdge(UserNId, rate)){
			net->AddEdge(UserNId, rate);
		}
		if(!net->IsEdge(rate, MovieNId)){
			net->AddEdge(rate, MovieNId);
		}
		if(!net->IsEdge(UserNId, MovieNId)){
			net->AddEdge(UserNId, MovieNId);
		}
		else { DupEdge++; 
		}

		if (++NLines % 100000 == 0) printf("\r  %dk  ", NLines/1000);
	}
		fclose(F);
		//fclose(hashfile);
		
        printf("duplicate edges: %d\n", DupEdge);
        printf("nodes:  %d\n", net->GetNodes());
        printf("edges:  %d\n", net->GetEdges());
        printf("users: %d\n", UserNIdH.Len());
        printf("movies: %d\n", MovieNIdH.Len());
		
			std::ostringstream out;
			out<<"../ml-1m/imdbkey1.txt";			
			FILE *outf = open_file(out.str().c_str(), "w");
			for(int i = 0; i < n1.Len(); i++){
				fprintf(outf, "%d\n", n1[i]);
			}
			fclose(outf);

			std::ostringstream out2;
			out2<<"../ml-1m/imdbkey2.txt";			
			outf = open_file(out2.str().c_str(), "w");
			for(int i = 0; i < n2.Len(); i++){
				fprintf(outf, "%d\n", n2[i]);
			}
			fclose(outf);

			std::ostringstream out3;
			out3<<"../ml-1m/imdbkey3.txt";			
			outf = open_file(out3.str().c_str(), "w");
			for(int i = 0; i < n3.Len(); i++){
				fprintf(outf, "%d\n", n3[i]);
			}
			fclose(outf);

			std::ostringstream out4;
			out4<<"../ml-1m/imdbkey4.txt";			
			outf = open_file(out4.str().c_str(), "w");
			for(int i = 0; i < n4.Len(); i++){
				fprintf(outf, "%d\n", n4[i]);
			}
			fclose(outf);
		
		return net;
}

void TMyNetNode::Save(TSOut& SOut) const {
	UserId.Save(SOut); Age.Save(SOut); IsMaleUser.Save(SOut); 
	Occupation.Save(SOut); Zipcode.Save(SOut); MovieId.Save(SOut);
	//Rating.Save(SOut); Timestamp.Save(SOut); 
	Title.Save(SOut); Videoreleasedate.Save(SOut); URL.Save(SOut); Genres.Save(SOut); Type.Save(SOut);
}

PMyNet TMyNet::LoadTxt_Gsc(const TStr& GscNm){
	PMyNet net = TMyNet::New();

	FILE *file = fopen(GscNm.CStr(), "rt");
	char line[2024];
	TIntH UserNIdH;
	TIntH MovieNIdH;
	int NLines=0, DupEdge=0, UserNId, MovieNId;
	int a, b;
	fgets(line, 2024, file);

	while(!feof(file)){
		memset(line, 0, 2024);
		fscanf(file, "%d%d", &a, &b);
		if (UserNIdH.IsKey(a)){
			UserNId = UserNIdH.GetDat(a);
		}
		else {
			UserNId = net->AddNode(a);
			UserNIdH.AddDat(a, UserNId);
		}
		if (MovieNIdH.IsKey(b)){
			MovieNId = MovieNIdH.GetDat(b);
		}
		else {
			MovieNId = net->AddNode(b);
			MovieNIdH.AddDat(b, MovieNId);
		}

		if (!net->IsEdge(a, b)){
			net->AddEdge(a, b);
		}else { DupEdge++; 
		}

		if (++NLines % 100000 == 0) printf("\r  %dk  ", NLines/1000);
	}

	fclose(file);

    printf("duplicate edges: %d\n", DupEdge);
    printf("nodes:  %d\n", net->GetNodes());
    printf("edges:  %d\n", net->GetEdges());
    printf("users: %d\n", UserNIdH.Len());
    printf("movies: %d\n", MovieNIdH.Len());

	return net;
}