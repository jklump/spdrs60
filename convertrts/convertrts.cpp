// Converts old format routing files (SpDrS60 for Linux <= v0.3.4)
// into new format ( >= v0.4)
// written by Stefan Preis, September 2nd, 2001
// changed by Guido Scholz, September 25nd, 2004

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main( int argc, char* argv[] )
{
   char line[80];
   int iTotal=0;

   FILE *file1;
   FILE *file2;
   
   /* guido */
   if (argc == 1) {
     printf("Usage:\n");
     printf("  convertRTS <sourcefile>\n");
     exit(1);
   }

   /* guido */
   if ((file1 = fopen(argv[1], "r")) == NULL) {
     fprintf(stderr, "%s: can't open %s\n", argv[0], argv[1]);
     exit(1);
   }

   do
   {
      fgets(line, 80, file1);
      if (strstr(line, "Route   # ------->") != NULL)
         iTotal += 1;
   }
   while (!feof(file1));
   fclose(file1);

   file1 = fopen(argv[1], "r");

   /* guido */
   if ((file2 = fopen("temp", "w")) == NULL) {
     fprintf(stderr, "%s: can't open %s\n", argv[0], "temp");
     exit(1);
   };

   int iRouteNoRead=0;

   fprintf( file2, "Routes total #: ?\n");
   fprintf( file2, "Last modified:  \n" );
   fprintf( file2, "version:        SpDrS60 v0.4.0\n");
   fprintf( file2, "-------------------------------------\n" );
   //---------------------------------------------------------------------------

   do
   {
      fgets( line, 80, file1 );
      if ( strstr( line, "Route   # ------->" ) != NULL )
      {
         iRouteNoRead += 1;
         fprintf( file2, "ROUTE --------> %d\n", iRouteNoRead );
      }
      else if( strstr( line, "-------------------------------------") != NULL )
      {
         fprintf( file2, "activate port:  -1\n");
         fprintf( file2, "active by loco: -1\n");
         fprintf( file2, "type:           0\n");
         fprintf( file2, "level:          -1\n");
         fprintf( file2, "data1:\n");
         fprintf( file2, "data2:\n");
         fprintf( file2, "data3:\n");
         fprintf( file2, "-------------------------------------\n");
         if( iRouteNoRead == iTotal )
            break;
      }
      else
         fprintf( file2, "%s", line );
   }
   while( !feof( file1 ) );
   fclose( file1 );
   fclose( file2 );

   sprintf( line, "mv temp %s", argv[1] );
   system( line );
}

