var ptz_jsForm=
{
 schema:
 {
  RAPIDECO: { type: 'integer', title: "Rapideco : ", default:"500" },
  "P": {
    type: "array",
    items:
    {
      type: "object",
      properties:
      {
        X:{ type: 'integer', title: "Pozicio : X = ", default:"0" },
        Y:{ type: 'integer', title: "Y = ", default:"0" },
  } } },
 },
 "form":[
  "RAPIDECO",
  "P[0]", "P[1]", "P[2]", "P[3]", "P[4]",
  {
    "type": "actions",
    "items": [ {
      "type": "submit",
      "value": "Submeti"
    } ]
  }
 ],
 onSubmit: function (errors, values) {
   if (errors)
   {
     $('#res').html('<p>Données incorrectes.</p>');
   }
   else
   { 
     // voir https://api.jquery.com/serializearray/
     var XHR = new XMLHttpRequest();
     var FD = JSON.stringify($("#ptz_form").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p> Changements enregistrés.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p> Erreur dans l enregistrement.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=ptz.conf");
     XHR.send("ptz_conf="+FD);
   }
 }
} ;

