var nokto_jsForm=
{
 schema:
 {
  "nivelo": { type: "range", title: "Nivelo : ", minimum: 0, maximum: 1000, },
  "histerezo": { type: "range", title: "Histerezo : ", minimum: 0, maximum: 100, },
  "intervalo": { type: "integer", title: "Intervalo inter du mezuroj :",min: 1, max:9999},
  "nombro": { type: "integer", title: "Nombro de mezuroj antaŭ lulado :"},
  irLed: { type: 'checkbox', title: "Aktivi IR LED :" },
  blankaLed: { type: 'checkbox', title: "Aktivi Blanka LED :" },
 },
 "form":[
    {"type": "help","helpvalue": "<strong>la servo «nokto» devas esti rekomencita por konsideri la ŝanĝojn.</strong>",htmlClass: "ml-n3 mt-2 mb-3 font-weight-bold text-primary"},
    {"type": "help","helpvalue": "<strong>Agordoj :</strong>",htmlClass: "ml-n3 mt-4 mb-1"},
    { key: "intervalo"},
    { key: "nivelo"},
    { key: "histerezo"},
    { key: "nombro"},
    {"type": "help","helpvalue": "<strong>Agoj :</strong>",htmlClass: "ml-n3 mt-4 mb-1"},
    { key: "irLed"},
    { key: "blankaLed"},
    {"type": "actions", "items": [ { "type": "submit", "value": "Submeti" } ],htmlClass: "mt-3" },
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
     var FD = JSON.stringify($("#noktoForm").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p>Ŝanĝoj konservitaj.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p>Eraro dum registrado.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=nokto.conf");
     XHR.send("nokto_conf="+FD);
   }
 }
} ;

