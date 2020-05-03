#include <EventBuilder/TEvent.h>

#include "TObject.h"
#include "TClass.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TObjArray.h"

#include "TEventHeader.h"

#include <cstring>
#include <iterator>
#include <string>
using namespace std;


TEvent::TEvent(){

	m_eventHeader=new TEventHeader();
	m_objects.SetOwner();
}

TEvent::~TEvent(){
	if (m_eventHeader) delete m_eventHeader;
}



TClonesArray* TEvent::getCollection (int id) const{
	if (id<this->getNcollections()){
		return m_collections[id];
	}
	else{
		Error("TEvent","TEvent::getCollection bad id!");
		return NULL;
	}
}

TClonesArray* TEvent::getCollection(TClass *theClass,string name) const{
	TClonesArray* ret=0;
	vector < TClonesArray* >::const_iterator it;
	for (it=m_collections.begin();it!=m_collections.end();it++){
		if ((strcmp((*it)->GetName(),name.c_str())==0)&&((*it)->GetClass()->InheritsFrom(theClass))){
			ret=*it;
			break;
		}
	}
	if (ret==0){
		Error("getCollection","Not found class: %s name:%s",theClass->GetName(),name.c_str());
	}
	return ret;
}


void TEvent::addCollection(TClonesArray *coll,int checkAlreadyExists){
	if (coll==NULL){
		Error("TEvent","TEvent::addCollection input null");
		return;
	}
	if (checkAlreadyExists){
		if (this->hasCollection(coll)){
			Warning("TEvent","TEvent::addCollection. Collection with same class and name already exists. Can't insert");
			return;
		}
	}
	m_collections.push_back(coll);
}


void TEvent::deleteCollection(TClass *theClass,string name){
	vector < TClonesArray* >::iterator it;
	for (it=m_collections.begin();it!=m_collections.end();it++){
		if ((strcmp((*it)->GetName(),name.c_str())==0)&&((*it)->GetClass()->InheritsFrom(theClass))){
			(*it)->Delete();
			m_collections.erase(it);
			return;
		}
	}
	Warning("TEvent","TEvent::deleteCollection not found class: %s name:%s",theClass->GetName(),name.c_str());
}




int TEvent::hasCollection(TClass *theClass,string name)const{
	int ret=0;
	vector < TClonesArray* >::const_iterator it;
	for (it=m_collections.begin();it!=m_collections.end();it++){
		if  ((strcmp((*it)->GetName(),name.c_str())==0)&&((*it)->GetClass()->InheritsFrom(theClass))){
			ret=1;
			break;
		}
	}
	return ret;
}


int TEvent::hasCollection(TClonesArray *coll)const{
	int ret=0;
	ret=this->hasCollection(coll->GetClass(),coll->GetName());
	return ret;
}

void TEvent::printCollections()const{
	vector < TClonesArray* >::const_iterator it;
	cout<<"Available collections: "<<m_collections.size()<<"class name - collection name"<<endl;
	for (it=m_collections.begin();it!=m_collections.end();it++){
		cout<<(*it)->GetClass()->GetName()<<" "<<(*it)->GetName()<<" TClonesArrayPointer: "<<(*it)<<endl;
		cout<<"Entries: "<<(*it)->GetEntries()<<endl;
	}
}




void TEvent::Clear(Option_t* opt){
	vector < TClonesArray* >::iterator it;
	for (it=m_collections.begin();it!=m_collections.end();it++){
		if (*it){
//			(*it)->Delete();
//			cout<<(*it)->GetName()<<endl;
			(*it)->Clear(opt);
			delete (*it);
		}
	}
	m_collections.clear();
	m_objects.Delete();

}


void TEvent::printObjects() const{
	TIter it(&m_objects);
	while (TObject* obj = it.Next()) {
		obj->Print();
	}
}





void TEvent::addObject(TObject *obj){
	m_objects.Add(obj);
}



TObject* TEvent::getObject(string name) const{
	TObject *ret=0;
	ret=m_objects.FindObject(name.c_str());
	return ret;
}

int TEvent::hasObject(string name) const{
	TObject *ret=0;
	ret=m_objects.FindObject(name.c_str());
	if (ret) return 1;
	else return 0;
}

