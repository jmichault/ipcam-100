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
      type:    { type: 'string', title: "interfaco :", default: "I2C", enum: [ "I2C","SPI"] },
      nom:     { type: 'string', title: "nomo de sensilo :", default: "gc2053", },
      adresse: { type: 'integer', title: "adreso de sensilo", default: "55", },
      largeur: { type: 'integer', title: "larĝo", default: "1920", },
      hauteur: { type: 'integer', title: "alteco", default: "1080", },
    }
   },
   "canal":
   {
    type: "array",
    items:
    {
      type: "object",
      title: ""videotrajtoj ,
      properties:
      {
        taille: { type: 'string', title: "bildograndeco (sekundara kanalo devas esti malpli granda)", enum : [ "1920x1080","1280x720","640x360" ] },
        format: { type: 'string', title: "video-formato", enum : [ "MJPEG","FixedQP","CBR","VBR","Smart","INV" ], default:"Smart" },
        maxqp: { type: 'integer', title: "maxQP (pli malalta = pli bona kvalito)", default:48 },
        minqp: { type: 'integer', title: "minQP (pli malalta = pli bona kvalito)", default:15 },
        bitrate: { type: 'integer', title: "bitrapido (kbps)", default:600 },
        fps: { type: 'integer', title: "nombro da kadroj sekunde", default:12, maximum:60, minimum:1 },
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
      title: "ĉefa kanalo",
      type: "tab"
      ,items:
      [{
        key: "canal[1]",
      }]
    },
    {
      title: "duaranga kanalo",
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
              "value": "Submeti"
            }
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
     var FD = JSON.stringify($("#myForm").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p>Ŝanĝoj konservitaj.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p>Eraro dum registrado.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=rtsp.conf");
     XHR.send("var config="+FD);
   }
 }
} ;

