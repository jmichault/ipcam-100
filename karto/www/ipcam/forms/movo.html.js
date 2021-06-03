var movo_jsForm=
{
 schema:
 {
  "on": { type: "checkbox", title: "Detekto Aktivida : ", },
  "sentemo": { type: "range", title: "Sentemo : ", minimum: 0, maximum: 4, },
  "FinaTempo": { type: "integer", title: "Fina Tempo"},
  "SenaktivaTempo": { type: "integer", title: "Senaktiva Tempo"},
  "MinZonoj": { type: "integer", title: "Minimuma nombro de zonoj dum movado"},
  "MaxZonoj": { type: "integer", title: "Maksimuma nombro de samtempaj zonoj kun movado"},

  "StokPicOn": { type: "checkbox", title: "stoki bildojn :", },
  "StokMovOn": { type: "checkbox", title: "stoki filmetojn :", },
  "StokForOn": { type: "checkbox", title: "stoki sur fora servilo :", },

  "SciiMailOn": { type: "checkbox", title: "sendi retpoŝton :", },
  "SciiMailAl": { type: "string", title: "Al :", },
  "SciiTelOn": { type: "checkbox", title: "sendi «telegram» :", },
  "SciiTelToken": { type: "string", title: "Token :", },
  "SciiTelChatId": { type: "string", title: "ChatId :", },
 },
"form":
[
 {
   type: "tabs",
   id: "movotabs",
   items: [
    {
      title: "ĝeneralaj agordoj",
      type: "tab",
      htmlClass: "form-inline py-3 px-3 ", 
      items:
      [
       {key:"on", htmlClass: "py-3 px-3", fieldHtmlClass: "ml-2", },
       {key:"sentemo"},
       {key:"FinaTempo"},
       {key:"SenaktivaTempo"},
       {key:"MinZonoj"},
       {key:"MaxZonoj"},
      ]
    },
    {
      title: "Stokado",
      type: "tab",
      htmlClass: "form-inline py-3 px-3 ", 
      items:
      [
       {key:"StokPicOn"},
       {key:"StokMovOn"},
      ]
    },
    {
      title: "Sciigo",
      type: "tab",
      htmlClass: "form-inline py-3 px-3 ", 
      items:
      [
       { type:"fieldset", items:[
        { key: "SciiMailOn"},
        { key: "SciiMailAl"},
       ]},
       { type:"fieldset", items:[
        {key:"SciiTelOn"},
        {key:"SciiTelToken"},
        {key:"SciiTelChatId"},
        {"type": "help", "helpvalue": '<ul><li>Serĉu @Botfather en «telegram»</li><li>krei novan «bot» kun «/newbot»</li><li>Botfather prezentos al vi la «token»</li><li>Kreu novan Grupon kaj enmetu la kreitan «bot» enen</li><li>Nun skribu ion al la Grupo</li><li>Voku https://api.telegram.org/bot[token)/getUpdates en via retumilo kaj serĉu «{"chat":{"id":».</li></ul>',},
       ]},
      ]
    },
    {
      title: "Areoj",
      type: "tab",
      htmlClass: "form-inline py-3 px-3 ", 
      items:
      [
    {
      "type": "help", "helpvalue": '<div id="Areoj" class="tabcontent"> <h2>Selektu rektangulojn por movdetekto</h2> <br> <div class="image-decorator"> <img alt="Image principale" id="ekrankopio" width="640" height="368" src="cgi-bin/currentpic.cgi" onerror="this.src="img/unable_load.png";"/> </div> <table> <tr> <td class="actions"> <input type="button" id="btnView" value="Display areas" class="actionOn" /> <input type="button" id="btnNew" value="Add New" class="actionOn" /> <input type="button" id="btnReset33" value="Reset to 3x3" class="actionOn" /> <input type="button" id="btnReset22" value="Reset to 2x2" class="actionOn" /> </td> </tr> </table> ',
    },
      ]
    },
   ]
 },
 {
  "type": "actions",
  "items": [
   { "type": "submit", "value": "Submeti" }
  ]
 }
],
 onSubmit: function (errors, values) {
          var XHR = new XMLHttpRequest();
          XHR.addEventListener("load", function(event) {
              $('#output').html('<p> Changements enregistrés.</p>');
            });
          XHR.addEventListener("error", function(event) {
              $('#output').html('<p> Erreur dans l enregistrement.</p>');
            });
          XHR.open("POST", "cgi-bin/conf_sav.cgi?file=movo.conf");
          var textRect=JSON.stringify($('img#ekrankopio').selectAreas('areas'));
          var textDetekto = JSON.stringify($("#movoForm").serializeArray());
          XHR.send( 'var Rektoj=' + textRect + ';\nvar DetektoDatenoj='+ textDetekto + ';\n' );
       }
} ;

