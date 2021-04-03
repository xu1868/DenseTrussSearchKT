#ifndef _MYNET_H
#define _MYNET_H

#include "Snap.h"
#include "Utility.h"
#include <cstdlib>

class TMyNet;
typedef TPt<TMyNet> PMyNet;

typedef enum { mtyUndef, mtyUser, mtyMovie} TItemTy;

class TMyNetNode{
private:
	TInt UserId, Age, Occupation, Zipcode;
	TBool IsMaleUser;
	TCh Type, Rating; TInt MovieId, Timestamp;
	TInt Title, Videoreleasedate, URL, Genres;
public:
	TMyNetNode():Type(mtyUndef) { }
	TMyNetNode(const int& UserId, const int& UserAge, const bool& IsMaleUser, const int& UserOccupation, const int& UserZipcode);
	TMyNetNode(const int& MovieId, const int& MovieTitle, const int& Videoreleasedate, const int& URL, const int& Genres);
	TMyNetNode(TSIn& SIn);
	void Save(TSOut& SOut) const;

	//user
	int GetUserId() const{ return UserId; }
	int GetAge() const{ return Age; }
	bool IsMale() const{ return IsMaleUser; }
	TItemTy GetTy() const { return TItemTy(Type.Val);}
	int GetOcc() const{ return Occupation; }
	int GetZip() const{ return Zipcode; }
	//data
	char GetRating() const{ return Rating; }
	int GetTime() const{ return Timestamp; }
	//moive
	int GetMovieId() const{ return MovieId; }
	int GetMovieTitle() const{ return Title; }
	int GetDate() const{ return Videoreleasedate; }
	int GetURL() const{ return URL; }
	int GetGen() const{ return Genres; }

	bool IsUser() const { return GetTy() == mtyUser; }
    bool IsMovie() const { return GetTy() != mtyMovie; }

	friend class TMyNet;
};

/*class TMyNetEdge{
private:
	TInt Scr, Dest, weight;
public:
	TMyNetEdge(const int& Scr, const int& Dest, const int& weight);
	TMyNetEdge(TSIn& SIn);
	void Save(TSOut& SOut) const;

	friend class TMyNet;
};*/

class TMyNet : public TNodeNet<TMyNetNode>{
public:
	typedef TNodeNet<TMyNetNode> TNet;
	typedef TPt<TNodeNet<TMyNetNode> > PNet;
private:
	TStrHash<TInt> StrIdH;
public:
	TMyNet() : StrIdH() { }
    TMyNet(TSIn& SIn, const bool& LoadStrH=true) : TNet(SIn) {
		if (LoadStrH) StrIdH.Load(SIn); printf("load.\n"); }
    void Save(TSOut& SOut) const { TNet::Save(SOut);  StrIdH.Save(SOut); }
    static PMyNet New() { return new TMyNet(); }
    static PMyNet Load(TSIn& SIn, const bool& LoadStrH=true) {
    return new TMyNet(SIn, LoadStrH); }
    // strings
    int GetStrs() const { return StrIdH.Len(); }
    bool IsStr(const TStr& Str) const { return StrIdH.IsKey(Str); }
    bool IsStr(const TStr& Str, int& StrId) const { StrId=StrIdH.GetKeyId(Str); return StrId!=-1; }
    int GetStrId(const TStr& Str) const { return StrIdH.GetKeyId(Str); }
    TStr GetStr(const int& StrId) const { IAssert(! StrIdH.GetPool().Empty()); return StrIdH.GetKey(StrId); }
    int AddStr(const TStr& Str) { return StrIdH.AddKey(Str); }

	PUNGraph GetItemGraph() const;
	//static PMyNet LoadTxt(const TStr& FNm, const TStr& MovieFNm, TIntV& nv1, TIntV& nv2, TIntV& nv3, TIntV& nv4);
	static PMyNet LoadTxt(const TStr& FNm, const TStr& MovieFNm, TIntV& n1, TIntV& n2, TIntV& n3, TIntV& n4);
	static PMyNet LoadTxt_Gsc(const TStr& GscNm);
	static PMyNet LoadUserNodeAttr(PMyNet& Net, const TStr& FNm);
	static PMyNet LoadMovieNodeAttr(PMyNet& Net, const TStr& FNm);
	static TItemTy GetItemTy(const TStr& ItemTyStr);
    static TStr GetItemTyStr(const TItemTy& ItemTy);
	friend class TPt<TMyNet>;
	
};

TMyNetNode::TMyNetNode(const int& UserId, const int& UserAge, const bool& IsMaleUser, const int& UserOccupation, const int& UserZipcode) :
	UserId(UserId), Age(UserAge), IsMaleUser(false), Occupation(UserOccupation), Zipcode(UserZipcode), Type(mtyUser) { }

TMyNetNode::TMyNetNode(const int& MovieId, const int& MovieTitle, const int& Videoreleasedate, const int& URL, const int& Genres):
	MovieId(MovieId), Title(MovieTitle), Videoreleasedate(Videoreleasedate), URL(URL), Genres(Genres){ }

TMyNetNode::TMyNetNode(TSIn& SIn) : UserId(SIn), Age(SIn), IsMaleUser(SIn), Occupation(SIn), Zipcode(SIn), MovieId(SIn), Rating(SIn), Timestamp(SIn), 
	Title(SIn), Videoreleasedate(SIn), URL(SIn), Genres(SIn),Type(SIn) { }
/*TMyNetEdge::TMyNetEdge(const int& Scr, const int& Dest, const int& weight):
Scr(Scr), Dest(Dest), weight(weight) { }
TMyNetEdge::TMyNetEdge(TSIn& SIn) : Scr(SIn), Dest(SIn), weight(SIn) { }*/


PUNGraph TMyNet::GetItemGraph() const{
	TIntPrSet EdgeSet;
	for (TNodeI NI = BegNI(); NI < EndNI(); NI++){
		if(NI().GetTy() == mtyUser){
			const int NId1 = NI.GetId();
		for (int e = 0; e < NI.GetOutDeg(); e++){
			if(NI.GetOutNDat(e).GetTy() != mtyUser){
				TNodeI NI2 = GetNI(NI.GetOutNId(e));
				for (int e2 = 0; e2 < NI2.GetInDeg(); e2++) {
					if(NI2.GetInNDat(e2).GetTy() == mtyUser) {
						const int NId2 = NI2.GetInNId(e2);
						EdgeSet.AddKey(TIntPr(TMath::Mn(NId1, NId2), TMath::Mx(NId1, NId2)));
				}
		}
		}
	}
}
}
	PUNGraph g = TUNGraph::New();
	for(int i = 0; i < EdgeSet.Len(); i++){
		const int NId1 = EdgeSet[i].Val1;
		const int NId2 = EdgeSet[i].Val2;
		if(!g->IsNode(NId1)) { g->AddNode(NId1);}
		if(!g->IsNode(NId2)) { g->AddNode(NId2);}
		g ->AddEdge(NId1, NId2);
	}
	return g;
}

#endif