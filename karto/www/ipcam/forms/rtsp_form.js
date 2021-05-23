var rtsp_jsForm=
{
 schema:
 {
   "sensor":
   {
    type: "object",
    title: "capteur",
    properties:
    {
      type:    { type: 'string', title: "interface", default: "I2C", enum: [ "I2C","SPI"] },
      nom:     { type: 'string', title: "nom du capteur", default: "gc2053", },
      adresse: { type: 'integer', title: "adresse du capteur", default: "55", },
      largeur: { type: 'integer', title: "largeur", default: "1920", },
      hauteur: { type: 'integer', title: "hauteur", default: "1080", },
    }
   },
   "canal":
   {
    type: "array"
    ,items:
    {
      type: "object",
      title: "propriétés de la vidéo",
      properties:
      {
        taille: { type: 'string', title: "taille de l'image (le canal secondaire doit avoir une taille plus petite)", enum : [ "1920x1080","1280x720","640x360" ] },
        format: { type: 'string', title: "format vidéo", enum : [ "MJPEG","FixedQP","CBR","VBR","Smart","INV" ], default:"Smart" },
        maxqp: { type: 'integer', title: "maxQP (plus bas = meilleure qualité)", default:48 },
        minqp: { type: 'integer', title: "minQP (plus bas = meilleure qualité)", default:15 },
        bitrate: { type: 'integer', title: "débit en kbps", default:600 },
        fps: { type: 'integer', title: "nb d'images par secondes", default:12, maximum:60, minimum:1 },
      }
    }
   }
 },
 form:
 [{
    key:"sensor",
  },
  {
   type: "tabs"
  ,id: "tabs"
  ,items: 
    [{
      title: "canal principal",
      type: "tab"
      ,items:
      [{
        key: "canal[1]",
      }]
    },
    {
      title: "canal secondaire",
      type: "tab"
      ,items:
      [{
        key: "canal[2]",
      }]
    },
   ]
 },
        {
          "type": "actions",
          "items": [
            {
              "type": "submit",
              "value": "Enregistrer"
            }
          ]
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
     var FD = JSON.stringify($("#myForm").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p> Changements enregistrés.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p> Erreur dans l enregistrement.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=rtsp.conf");
     XHR.send("var config="+FD);
   }
 }
} ;

