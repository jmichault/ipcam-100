# gawk-skripto por traduki po-dosieron el esperanto al alia lingvo
# la ideo estas prilabori MSGIDojn por kiuj la MSGSTR estas malplena.
# MSGID-oj, kiuj havas Aferon inter layout, lang-ref, lang-niv, Fenced ne estas tradukitaj.
# Ni tiam eltranĉas la tradukenda tekston por forigi la elementojn de la sintakso ~ Markdown ~
# hiperligoj, kodblokoj kaj teksto kursivigita per "_" ne estas tradukitaj.
#
# google web translate konservas  «(°99°)».
# 
BEGIN {
#  MARK1="(zzz";
#  MARK2=")";
#  MARKEXPR= " *[（\\(] *[zZ]{2,3} *[0-9]+ *[）\\)] *";
  if ( (dst == "hi") )
  {
    MARK1="(°000";
    MARK2="°)";
    MARKEXPR=" *[（\\(][ \\.。°]*[0-9]*[ \\.。°]*[）\\)] *";
  }
  else
  {
    MARK1="(°";
    MARK2="°)";
    MARKEXPR=" *[（\\(][ \\.。°0-9]*[°][ \\.。°0-9]*[）\\)] *";
  }
}
{
  if ( CONTMSG==1 && substr($1,1,1) != "\"")
  { # fino de plurlinia mesaĝo
    CONTMSG=0;
  }
  if ($2 == "fuzzy")
  {
    FUZZY=1;
  }
  if ($1 == "msgid")
  {
    MSGID=substr($0,7);
    MSGSTR=substr($0,8,length($0)-8);
    MSGWRAP=0;
    if(MSGID=="\"\"")
    {
      CONTMSG=1;
      MSGWRAP=1;
    }
  }
  else if ( CONTMSG==1 && substr($1,1,1) == "\"")
  { # daŭrigo de plurlinia mesaĝo
    # enmemorigi MSGID-liniojn
    MSGID=MSGID "\n" $0;
    # enmemorigi netradukitan mesaĝon
    MSGSTR=MSGSTR substr($0,2,length($0)-2);
  }
  else if ($1 == "msgstr")
  {
    if( ($2 != "\"\"" && $2 != "\"\\n\"") || MSGID == "\"\"")
    { # mesaĝo jam tradukita
      print ("msgid " MSGID);
      print $0;
    }
    else
    {
      # msgstr "" kaj MSGID != ""
      $0="";
      getline
      if ( substr($1,1,1) == "\"")
      { # plurlinia mesaĝo jam tradukita
        print ("msgid " MSGID);
        printf("msgstr \"\"\n");
        print $0;
        FUZZY=0;
        MATTER="";
	next;
      }
      if(MATTER == "lang")
      {
        print ("msgid " MSGID);
        printf("msgstr \"" dst "\"\n");
      }
      else if(MATTER == "layout")
      {
        print ("msgid " MSGID);
        print ("msgstr " MSGID);
      }
      else if(MATTER == "lang-ref")
      {
        print ("msgid " MSGID);
        print ("msgstr " MSGID);
      }
      else if(MATTER == "lang-niv")
      {
        print ("msgid " MSGID);
        print ("msgstr \"auto\"" );
      }
      else if(MATTER == "Fenced")
      {
        print ("msgid " MSGID);
        print ("msgstr " MSGID);
      }
      else
      { # ĉi tiu mesaĝo estas tradukenda
        if(FUZZY ==0)
          print ("#, fuzzy");
        print ("msgid " MSGID);
        printf("msgstr \"");
	#  anstataŭigi markdown-etikedojn per markoj, kaj jekyll
	########## construction de l'expression régulière pour split
	########## note : contrairement à l'intuition, ce sont les dernières expressions qui sont traitées en premier
	### protection du code markdown
	# markdown : tous les caractères spéciaux avec les espaces avant et après
	regexp=      "[ \t]*[_\\*`<>\\[\\]\\(\\)~]+[ \t]*"; #
	regexp=regexp"|^[ \t]*";        # markdown : espaces en début de ligne
	regexp=regexp"|[ \t]*\\\\\\\\."; # markdown : espaces+\\x
	regexp=regexp"|[ \t]*\\\\.";    # markdown : espaces+\x
	regexp=regexp"|[ \t]*!\\[[ \t]*"; # markdown : espaces![espaces : début d'un lien
	### protection du code jekyll
	### on repére le début et la fin, on bouclera pour protéger tout le bloc
	regexp=regexp"|{{ *| *}}|{% *| *%}"; 
	### protection d'une partie du html
	regexp=regexp"| */>";         # fin de balise html
        regexp=regexp"| *<(img) +(title=|alt=)*"; # balises html à protéger
        regexp=regexp"| +(title|alt)="; # mots clé html à protéger
	regexp=regexp"| +(href|ref|style|src)=\\\\\"[^\"]*\\\\\""; # attributs html à protéger
	regexp=regexp"| *</ *a *>";         # fin de balise html <a>
	regexp=regexp"| *<a *[^>]*";      # balise <a ... multiligne : on protège la première ligne.
	regexp=regexp"| *<a *[^>]*>";            # balise html <a ....>, attention : pb si \> à l'intérieur.

        split(MSGSTR,MSGS,regexp,SEPS);

	MSG0 = "";
	MSGSLEN = length(MSGS);
        ##print ("MSGSLEN="MSGSLEN); ##
        ##for (x=0 ; x<=MSGSLEN ; x++)		##
	##	print ("MSGS "x " : " MSGS[x] ". sep " x " : "  SEPS[x]".");	##
        if(MSGSLEN==1) MSG0 = MSGSTR; # cimon en «split» : unua signo perdita ?
        else for (x=1 ; x<=MSGSLEN ; x++)
        {
	  MSG0 = MSG0 MSGS[x] ;
	  ##print "x = " x;	##
	  if (SEPS[x] != "")
	  {
	    MSG0 = MSG0 " " MARK1 x MARK2 " ";
	    if( match(SEPS[ x ] ,"{%") == 1 )
	    { # ne traduku «{% ...%}» : jekyll
	      x0 = x ;
	      do
              {
                x++;
                SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
              }
              while( x<=MSGSLEN && ! match(SEPS[x] ,"%}"));
	    }
	    else if( match(SEPS[ x ] ,"{{") == 1 )
	    { # ne traduku «{{ ... }}» : jekyll
	      x0 = x ;
	      do
              {
                x++;
                SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
              }
              while( x<=MSGSLEN && ! match(SEPS[x] ,"}}"));
	    }
	    else if( match(SEPS[ x ] ,"^<a *id *=") == 1 )
	    { # ne traduku «<a» HTML-etikedo
	      x0 = x ;
	      do
              {
                x++;
                SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
              }
              while( x<=MSGSLEN && ! match(SEPS[x] ,"</ *a>"));
	    }
	    # ne traduku hiperligon, kodon, kursivigita kun "_"
	    else if( match(SEPS[ x ] ,"^ *`") == 1 )
	    { # kodo : ne traduku.
	      x0 = x ;
	      do
              {
                x++;
                SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
              }
              while( x<=MSGSLEN && ! match(SEPS[x] ,"`"));
	    }
	    else if( match(SEPS[ x ] ,"^ *_$") == 1 )
	    { # kursivigita kun "_" : ne traduku
	      x0 = x ;
	      do
              {
                x++;
                SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
              } while( (x <= MSGSLEN) && (match(SEPS[x] ,"^_[ \t]*")==0));
	    }
	    else  if( match(SEPS[ x ] ," *\\]\\( *") )
	    { # hiperligo : traduku teksto,ne traduku ligo.
	      x0 = x ;
                ## print ("hiperligo  :" SEPS[ x ]);	##
	        do
                {
                  x++;
                  SEPS[x0] = SEPS[x0] MSGS[x] SEPS[x];
                }
                while( x<=MSGSLEN && ! match(SEPS[x] ," *\\) *"));
	    }
	  }
	}
        ##print("\nMSG0 " MSG0);	##
        BASEDIR"/traduko.sh " src " " dst " \"" MSG0 "\"" |getline MSG
        ##print("MSG " MSG);		##
        ##print("MSGSLEN=" MSGSLEN);    ##
	split(MSG, MSGS2, MARKEXPR ,SEPS2);
	MSGT="";
        DOSEPMAX=0;
        for (x=1 ; x<=length(MSGS2) ; x++)
	{ # anstataŭigi markoj per markdown-etikedojn
	  MSGT = MSGT MSGS2[x];
	  if (SEPS2[x] != "")
	  {
	    match(SEPS2[x],"[1-9][0-9]*");
	    x2 = substr(SEPS2[x],RSTART,RLENGTH);
	    ##print("x2=" x2);		##
	    if (x2 == (MSGSLEN-1) && match(SEPS[x2]," *\\\\n" ))
              DOSEPMAX=1
            else
              MSGT = MSGT SEPS[x2];
          }
	}
	if (match(SEPS[MSGSLEN-1]," *\\\\n" ))
          MSGT = MSGT SEPS[MSGSLEN-1];
        print(MSGT "\"");
      }
      print $0;
    }
    FUZZY=0;
    MATTER="";
  }
  else if (substr($0,1,28) == "#. type: YAML Front Matter: ")
  {
    MATTER=substr($0,29);
    print $0;
  }
  else if (substr($0,1,15) == "#. type: Fenced")
  {
    MATTER="Fenced";
    print $0;
  }
  else
  {
    print $0;
  }
}
