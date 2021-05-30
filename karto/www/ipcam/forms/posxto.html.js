var posxto_jsForm=
{
 schema:
 {
  adresanto: { type: 'string', title: "Adresanto :"},
  ensaluto: { type: 'string', title: "Ensaluto :"},
  pasvorto: { type: 'string', title: "Pasvorto :"},
  servilo: { type: 'string', title: "Servilo :"},
  haveno: { type: 'integer', title: "Haveno :", default:465},
  testoAl: { type: 'string', title: "Al :",default:"x@y.com"},
  testoPri: { type: 'string', title: "Pri :",default:"Alarmo de kamerao"},
  testoMesagxo: { type: 'string', title: "Mesaĝo :",default:"Jen atentigo de via kamerao"},
 },
 form:
 [
  { key: "adresanto"},
  {"type": "help","helpvalue": "<strong>Servilo (SSL deviga) :</strong>",htmlClass: "ml-n2 mt-2"},
  { type:"fieldset",
      htmlClass: "inline py-3 px-3 ", 
    items:[
    { key: "servilo"},
    { key: "haveno"},
  ] },
  {"type": "help","helpvalue": "<strong>Aŭtentigo :</strong>",htmlClass: "ml-n2 mt-2"},
  { type:"fieldset",
      htmlClass: "inline py-3 px-3 ", 
    items:[
    { key: "ensaluto"},
    { key: "pasvorto"},
  ] },
  {"type": "help","helpvalue": "<strong>Testo :</strong>",htmlClass: "ml-n2 mt-2"},
  { type:"fieldset",
      htmlClass: "inline py-3 px-3 ", 
    items:[
    { key: "testoAl"},
    { key: "testoPri"},
    { key: "testoMesagxo"},
  ] },
  {
    "type": "actions",
    htmlClass: "inline py-3 px-3 ",
    "items": [
    {
      "type": "submit",
      "title": "Submeti",
      "id":"posxtoSubmeti",
    },
    {
      "type": "button",
      htmlClass: "ml-3",
      "title": "Submeti kaj Testi",
      "onClick": function (evt) {
        $("#posxtoSubmeti").click();
        alert('Testo ne implementa!');       
       }
    },
  ]
  }
],
 onSubmit: function (errors, values) {
   if (errors)
   {
     $('#res').html('<p>Malĝustaj datumoj.</p>');
   }
   else
   { 
     // voir https://api.jquery.com/serializearray/
     var XHR = new XMLHttpRequest();
     var FD = JSON.stringify($("#posxtoForm").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p>Ŝanĝoj konservitaj.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p>Eraro dum registrado.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=posxto.conf");
     XHR.send("posxto_conf="+FD);
   }
 }
} ;

