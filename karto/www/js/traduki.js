var loki18n=undefined;
function lokGet(yourUrl){
  var Httpreq = new XMLHttpRequest(); // a new request
  Httpreq.open("GET",yourUrl,false);
  Httpreq.send(null);
  return Httpreq.responseText;
}

function lokLoadJson(base,lang)
{
  i18njson=lokGet("lok/"+base+"."+lang+".json");
  try 
  {
    loki18n.loadJSON(i18njson, base);
    return true;
  }
  catch(e)
  {
    return false;
  };
}

function setPageLang(base,source){
  if(loki18n==undefined)
    loki18n=window.i18n();
  var cookLang=Cookies.get("lang");
  var userLang="eo";
  if( cookLang == "eo")
  {
    loki18n.loadJSON( '{"":{"language":"eo","plural-forms":"nplurals=2; plural=(n > 1);"}}',base);
  }
  else if( cookLang && lokLoadJson(base,cookLang ))
  {
    userLang = cookLang;
  }
  else 
  {
    var userLangs = navigator.languages ||
               [navigator.language] ||
               [navigator.userLanguage ];
    var i18njson, i;
    for (i = 0; i < userLangs.length ; i++)
    {
      if( lokLoadJson(base,userLangs[i].substring(0,2)) )
      {
        userLang = userLangs[i].substring(0,2);
        break;
      }
      else
      {
        continue;
      };
    }
  }
  loki18n.setLocale(userLang);
  loki18n.textdomain(base);
  source.find(".lang").each(function(){
    $(this).html(
	loki18n.gettext($(this).data("lang") )
	);});
  source.find(".plang").each(function()
   {
     var src=$(this).data("lang");
     var dst=loki18n.gettext(src);
     $(this).attr('placeholder' , dst);
   });
};

function setFormLang(data) {
  for(var key in data){
    if(key == 'title' || key == 'helpvalue')
      data[key]=loki18n.gettext(data[key]);
    if (data.hasOwnProperty(key)) {
      if (data[key] != null && typeof data[key] == 'object') {
        setFormLang(data[key]);
      }
    }
  }
}
