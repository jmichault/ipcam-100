var reto_jsForm=
{
 schema:
 {
  dns1: { type: 'string', title: "dns1 IP-adreso :", default:"8.8.8.8"},
  dns2: { type: 'string', title: "dns2 IP-adreso :", default:"8.8.4.4"},
  eth0_aktiva: { type: 'checkbox', title: "Aktivi :" },
  eth0_dhcp: { type: 'checkbox', title: "Uzi dhcp :" },
  eth0_ip: { type: 'string', title: "IP-adreso :" },
  eth0_retmasko: { type: 'string', title: "/" },
  eth0_enirejo: { type: 'string', title: "enirejo :" },
  wlan0_aktiva: { type: 'checkbox', title: "Aktivi :" },
  wlan0_dhcp: { type: 'checkbox', title: "Uzi dhcp :" },
  wlan0_ip: { type: 'string', title: "IP-adreso :" },
  wlan0_retmasko: { type: 'string', title: "/" },
  wlan0_enirejo: { type: 'string', title: "enirejo :" },
  wlan1_aktiva: { type: 'checkbox', title: "Aktivi :" },
  wlan1_dhcp: { type: 'checkbox', title: "Uzi dhcp :" },
  wlan1_ip: { type: 'string', title: "IP-adreso :" },
  wlan1_retmasko: { type: 'string', title: "/" },
  wlan1_enirejo: { type: 'string', title: "enirejo :" },
  wlan0_SSID: { type: 'string', title: "SSID :" },
  wlan0_pasvorto: { type: 'string', title: "WiFi pasvorto :" },
  wlan1_SSID: { type: 'string', title: "SSID :" },
  wlan1_pasvorto: { type: 'string', title: "WiFi pasvorto :" },
  wlan1_dhcpstart: { type: 'string', title: "Adresoj gamo :" },
  wlan1_dhcpend: { type: 'string', title: "-" },
 },
 form:
 [{
   type: "tabs",
   id: "retotabs",
   items: 
    [{
      title: "ĝeneralaj agordoj",
      type: "tab",
      htmlClass: "form-inline py-3 px-3 ", 
      items:
      [
       {"type": "help","helpvalue": "<strong>DNS Serviloj :</strong>",htmlClass: "ml-n2 mt-2"},
       {key:"dns1", fieldHtmlClass: "py-3 px-3 ", },
       {key:"dns2"},
      ]
    },
    {
      title: "LAN eth0",
      type: "tab",
      htmlClass: "form-inline py-4 px-4 ", 
      items:
      [
       { key: "eth0_aktiva"},
       {"type": "help","helpvalue": "<strong>IP Agordoj :</strong>",htmlClass: "ml-n3 mt-4"},
       { key: "eth0_dhcp"},
       {"type": "help","helpvalue": "<strong>manaj IP-agordoj (estos ignorita se dhcp estas ebligita) :</strong>",htmlClass: "ml-n1 mt-2 mb-1"},
       { type:"fieldset",
        items:[
        { key: "eth0_ip"},
        { key: "eth0_retmasko"},
        ]
       },
       { key: "eth0_enirejo"},
      ]
    },
    {
      title: "WiFi-kliento wlan0",
      type: "tab",
      htmlClass: "form-inline py-4 px-4 ", 
      items:
      [
       { key: "wlan0_aktiva"},
       {"type": "help","helpvalue": "<strong>IP Agordoj :</strong>",htmlClass: "ml-n3 mt-4"},
       { key: "wlan0_dhcp"},
       {"type": "help","helpvalue": "<strong>manaj IP-agordoj (estos ignorita se dhcp estas ebligita) :</strong>",htmlClass: "ml-n1 mt-2 mb-1"},
       { type:"fieldset",
        items:[
        { key: "wlan0_ip"},
        { key: "wlan0_retmasko"},
        ]
       },
       { key: "wlan0_enirejo"},
       {"type": "help","helpvalue": "<strong>Wifi Agordoj :</strong>",htmlClass: "ml-n3 mt-4"},
       { key: "wlan0_SSID"},
       { key: "wlan0_pasvorto"},
      ]
    },
    {
      title: "WiFi-AP wlan1",
      type: "tab",
      htmlClass: "form-inline py-4 px-4 ", 
      items:
      [
       { key: "wlan1_aktiva"},
       {"type": "help","helpvalue": "<strong>IP Agordoj :</strong>",htmlClass: "ml-n3 mt-4"},
       { type:"fieldset",
       items:[
        { key: "wlan1_ip"},
        { key: "wlan1_retmasko"},
        ]
       },
       { key: "wlan1_enirejo"},
       {"type": "help","helpvalue": "<strong>Wifi Agordoj :</strong>",htmlClass: "ml-n3 mt-4"},
       { key: "wlan1_SSID"},
       { key: "wlan1_pasvorto"},
       {"type": "help","helpvalue": "<strong>DHCP Servilo :</strong>",htmlClass: "ml-n3 mt-4"},
       { type:"fieldset",
        items:[
        { key: "wlan1_dhcpstart"},
        { key: "wlan1_dhcpend"},
        ]
       },
      ]
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
     var FD = JSON.stringify($("#retoForm").serializeArray());
     XHR.addEventListener("load", function(event) {
         $('#res').html('<p>Ŝanĝoj konservitaj.</p>');
       });

     XHR.addEventListener("error", function(event) {
         $('#res').html('<p>Eraro dum registrado.</p>');
       });

     XHR.open("POST", "cgi-bin/conf_sav.cgi?file=reto.conf");
     XHR.send("reto_conf="+FD);
   }
 }
} ;

