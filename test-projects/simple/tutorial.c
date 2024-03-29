#include <stdio.h>
int main() {
   printf("Hello, World!");
   return 0;
}

void there(int x, int y){
   main();
   there(main(), x);
}

void hello(){
   there(1 + main(), main() * 2);
}

int hello_there() {
   hello();
   there(5, 6 + main());
   return 0;
}

int checkin_it(){
   int b = 1;
   if(b==0){
      hello();
      b=2;
      there(b,b);
   }
   for(int i=0;i<15;i++){
      hello();
      there(3,3);
      there(3,3);
      there(3,3);
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
   }
}

int for_five(){
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
}

int while_while(){
   while(1){
      int i=0;
      1;
      2;
      3;
   }
   while(8){
      5;
   }
}

void compound_compound(){
   hello();
   hello();
   hello();
   {
      hello();
      hello();
   }

   there(1,1);

   {
      hello();
      hello();
   }

   {
      hello();
      hello();
   }
}

void run_if(){
   int b=4;
   if(b==0){
      hello();
      b=2;
      there(b,b);
   }else{
      hello();
   }
   hello();
   if(b==2)
      there(b,b);
   
   hello();
}


void almost(){
   int b = 2;
   switch(b){
      b = 8;
      b < 8;
      case 1:
      b+=2;
      break;

      case 900:
      b+=2;
      break;

      case 0:
      case 2:
      b=5;
      break;

      case 3:
      case 4:
      case 5:
      b = 0;
      default:
      {
         int q = 8;
         b = q;
      }
      
   }
}

void switch_in_switch(){
   int b = 2;
   switch(b){
      case 0:
      b = 8;
      break;
      case 1:
      switch(b){
         case 2:
         b=3;
         case 5:
         b=8;
      }
      break;
   }
}

void switch_with_garbage(){
   int b = 2;
   switch(b){
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      b = 8;
      case 0:
      b = 8;
   }
}

void normal_for(){
   int q=0;
   for(int i=0;i<10;i++){
      q+=i*i;
   }
}

void evil_for(){
   int q=0;
   for(int i=0;i<10;i++) q+=i*i;   
}

void broken_for(){
   int q=0;
   for(int i=0;i<10;i++){
      q+=i*i;
      if(q>8) break;
      if(i<3) continue;
      q=8;
   }
}

void normal_while(){
   int q=0;
   while (q<10)
   {
      q+=1;
   }
   q=8;
}

void evil_while(){
   int q=0;
   while(q<10) q+=1;
   q=8;
}

void broken_while(){
   int q = 0;
   while(q < 10){
      q += 1;
      if(q > 8) break;
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void while_with_unreachable_part(){
   int q = 0;
   while(q < 10 && 15>0){
      q += 1;
      if(q > 8){
         break;
      }else{
         return;
      }
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void while_with_return(){
   int q = 0;
   while(q < 10){
      q += 1;
      if(q > 8) return;
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void unreachable(){
   int q=0;
   return;
   q++;
   q++;
   q++;
}

void gotos(){
   int q=0;
   start: q+=1;
   q=3;
   q+=1;
   if(q==0){
      goto end;
   }else{
      goto start;
   }
   end:return;
}

void gotos_evil(){
   int q=0;
   start: q+=1;
   q=3;
   q+=1;
   if(q==0 && 7==(0+1+2+3+4))
      goto end;
   else
      goto start;
   end:return;

   while(1) goto end;
}

void many_logical_comparisons(){
  int b = 0, q = 0, w = 1;
  while(b==1 && q==4 || w==8){
    if(w<0 || w>0){
      for(int i=0;i<8;i++){
        b+=1;
      }
    }
  }
}

void no_care_for_returns(){
  if(8==8){
    return;
  }else{
    return;
  }
  if(9==9){
    for(int i=0;i<0;i++){
      if(8==8){
        9+4;
      }
    }
  }
}

void calculated_goto_rule(){
  int a,b,c,d,e;
  aa: a++;
  bb: b++;
  cc: c++;
  dd: d++;
  ee: e++;
  goto aa;
  void* omg = &&aa;
  goto *omg;
}


void calculated_goto_rule_two(){
  int a,b,c,d,e;
 aa: a++;
 bb: b++;
 cc: c++;
 dd: d++;
 ee: e++;
  goto aa;
  void* omg = &&aa;
  void* omgad = &&bb;
  goto *omgad;
  goto *omg;
}
