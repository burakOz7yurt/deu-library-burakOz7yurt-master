#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
#include <stdbool.h>

#define STUDENT_NUMBER 101//student sayısını direk burdan verebilirsiniz
//oda da 4den az öğrenci kalırsa onuda çıkardığımız görülsün diye 101 verdim
void *student(void *number);//student thread fonksiyonu
void *f_room(void *keep_number);//room thread fonksiyonu
int count_room_number();//yerleşilecek odayı secen fonksiyon

//indicate the condition of this room
sem_t room_student_number[10];
//waiting roomun semophoru
sem_t s_waiting_room;
//oda daki keeper ın çalışmasını konturol ediyor
sem_t keeper_run[10];
//öğrencinin çalışmasını kontrol ediyor
sem_t student_run[10];
//tüm öğrencilerin sayısını tutan semophore
sem_t number_of_allstudents;
 
 pthread_mutex_t student_lock;//oda da kaç öğrenci var diye sayılırken ve anons edilirken karışıklığı önler

int main()
{  
    
    int i=0;//forlar için

    pthread_t tid[STUDENT_NUMBER];//öğrenci threadleri için
    pthread_t Rtid[10];//oda threadleri için
    
    int Number[STUDENT_NUMBER];
    int room_number[10];
    for(i=0;i<10;i++)
    {
    sem_init(&room_student_number[i], 0, 4);//semophore değeri 4 iken odada sıfır öğrenci var
    }
    for(i=0;i<10;i++)
    {
    sem_init(&keeper_run[i],0,1);
    }
    for(i=0;i<10;i++)
    {
    sem_init(&student_run[i],0,0);
    }
    
    sem_init(&number_of_allstudents,0,STUDENT_NUMBER);
    sem_init(&s_waiting_room,0,STUDENT_NUMBER);

    for (i = 0; i < STUDENT_NUMBER; i++) {
        Number[i] = i;
    }
    for (i = 0; i < 10; i++) {
        room_number[i] = i;
    }
     for (i = 0; i < 10; i++) {//bu foru kullanırsam ilk önce oda threadleri create ediliyor oda threadlerini 
     //alttaki fordaki yorum satırındaki gibi create edersem öğrenci threadleriyle karışık create yapılıyor 
        pthread_create(&Rtid[i],NULL,f_room,(void *)&room_number[i]);
    }
    for (i = 0; i < STUDENT_NUMBER; i++) {
      
        pthread_create(&tid[i], NULL, student,(void *)&Number[i]);
       /* if(i<10)
        {
       pthread_create(&Rtid[i],NULL,f_room,(void *)&room_number[i]);

        }*/
    }
     
   for (i = 0; i < STUDENT_NUMBER; i++) {
        pthread_join(tid[i],NULL);
    }
   
   for (i = 0; i < 10; i++) {
        pthread_join(Rtid[i],NULL);
    }
  
     system("PAUSE"); 
    return 0;
}
void *f_room(void *room_sayisi)
{   
    int say=0;//oda daki öğrenci sayısını tuttuğum değişken buna göre keeper işlemler yapıyor
    int what_is_room = *(int *)room_sayisi;//what_is_room odanın kaçıncı oda olduğunu belirtiyor
    while(7>3)   
    {
    sem_wait(&keeper_run[what_is_room]);//ilkte aktif daha sonraları student tarafından aktifleştirilen semephore
     label://eğer oda boşaltıldıysa döngü bitmeden temizlik yapıldığını belirtmek için
    sem_getvalue(&room_student_number[what_is_room],&say);  
    if( say!=4 && say!=0)
    {
    
    printf("------------------room %d has space for %d students----------------\n",what_is_room,say);
    }
    else if(say==4 )
    {
        
     
        sleep(rand()%2);
        printf("****************room %d has cleaning******************* \n",what_is_room);
   }
   else if(say==0)
   {  
         printf("room %d full\n",what_is_room);
        sem_post(&room_student_number[what_is_room]);
        sem_post(&room_student_number[what_is_room]);
        sem_post(&room_student_number[what_is_room]);
        sem_post(&room_student_number[what_is_room]);
        sleep(4);
        printf("emptying room %d\n",what_is_room);
     
        goto label;   
   }
 
  int deg=0;//tüm öğrencilerin sayısını if de sorgulayabilmek için değişken
   sem_getvalue(&number_of_allstudents,&deg);
   if(deg==0)
   { 
       sem_post(&keeper_run[(what_is_room+1)%10]);//burada dögü kırılırken bu odadan bi sonraki odanında semophorunu aktifleştirmek
       //zorundayım çünkü diğer odaları aktifleştirecek öğrenci kalmadığı için diğer odalar beklemede kalıyor ve döngülerini kıramıyorlar
       //bu işlem odalar bitene kadar devam ediyor
       break;     
   }

   sem_post(&student_run[what_is_room]);
    
  }
  
  sem_getvalue(&room_student_number[what_is_room],&say);
  
  if(say!=4)//döngü kırılınca bu if e girerse oda da öğrenci var demektir ve gelecek öğrenci kalmadığı için odayı boşaltıp kapatmalıyız
  {
      int i=0;
      int student_number_in_room=4-say;
      printf("There are %d students in room %d the library is being closed.Whence, we have to empty the room\n",student_number_in_room,what_is_room);
      printf("emptying room %d \n",what_is_room);
      for(i=0;i<student_number_in_room;i++)//odayı boşaltma işlemi
      {
         sem_post(&room_student_number[what_is_room]);
      }
  }
   
  sleep(rand()%5);//odalar kilitlenirken görünsün diye
  printf("room %d are being locked and room keeper goes to home \n",what_is_room);
  

}
void *student(void *number) { 
     int num = *(int *)number; 
     int sem_deger=0;//oda daki öğrenci sayısını öğrenmek için
     int which_room=0;//hangi odaya girecek örenci
       sleep(rand()%60);//studentların waiting room a girişleri yavaş yavaş ve az az öğrenci girsin diye
       sem_wait(&s_waiting_room);
     printf("Student %d arrived at library and enter the waiting room.\n", num); //öğrenci bekleme odasında bekliyor..
     
   pthread_mutex_lock(&student_lock); //bir öğrenci bekleme salonundan odasını seçip girerken diğer öğrenciler bekliyor
     which_room=count_room_number();
     sem_wait(&student_run[which_room]);//keeper tarafından aktif edilir
     sem_getvalue(&room_student_number[which_room],&sem_deger);
     if(sem_deger==4)//oda da temizlik var demektir öğrenci oda görevlisini uyarır
     {
         printf("student %d alert roomkeeper in room %d\n",num,which_room);
     }
     sem_wait(&room_student_number[which_room]);
     
     sem_post(&s_waiting_room);
     printf("Student %d entering room%d\n", num,which_room); // buradaki room un 1 i parametre olarak alınacak
     sem_wait(&number_of_allstudents);//gelen öğrenciler odalara girdikçe onları tüm öğrencilerden çıkartıyorum
     sem_post(&keeper_run[which_room]);  //keeper ı aktifleştirmek için 
     sleep(2);//konsolda akış görülsün diye
    pthread_mutex_unlock(&student_lock); 
   
}
int count_room_number()//öğrenciye hangi odaya gideceğini söyleyen fonksiyonum
{
    int i=0;//forlar için
    int rast=0;//return değer while kırılınca 
    int min=4;//max öğrenci sayısı olan odanın sayısını tutacak ama ters işlem var yani semophorun değeri fazla olunca odada az yer kalmış oluyor
    int room_value_array[10];
    for(i=0;i<10;i++)
    {
        sem_getvalue(&room_student_number[i],&room_value_array[i]);
    }
    for(i=0;i<10;i++)//bu döngüde maximum sayıdaki odanın valuesı hesaplanıp max değerine atılıyor
    {
       if(room_value_array[i]<min && room_value_array[i]!=0)
       {
           min=room_value_array[i];
       }
    }
    while(7>3)//burada da odalar random olarak dolaşılarak maximum oda bulunuyor random yapmamın sebebi birden fazla max oda 
    //olduğunda farklı odalarda tercih edilebilsin diye ama max 1 taneyse öğrenci direk ona yönlendriliyor
    {
        rast=rand()%9;
        if(room_value_array[rast]==min)
       {
          break;
       }
    }
    return rast;
   
}