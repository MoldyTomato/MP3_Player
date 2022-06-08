char System_Volume_Information[255]="System Volume Information";
int NumOfIgnoredItems = 1;
char* IgnoredItems[255]= {System_Volume_Information};
bool isIgnored(const char* s){
  if(strcmp(s, "System Volume Information")==0){
    return true;
  }
  return false;
};
