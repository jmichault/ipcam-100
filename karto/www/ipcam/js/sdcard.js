
function getFiles(dir, subdir) {
 $('#files').html("");
 // Get files from dir
 $.get("cgi-bin/media.cgi", {cmd: "getFiles", dir: dir,subdir: subdir}, function(out){
  var out_all = out.split("\n");
  if ( out_all.length == 1)
   $('#files').html("<h1> Neniuj dosieroj trovitaj. </h1>");
  else
   $('#files').html("");
   for (var i = 0; i < out_all.length-1; i++) {
    var out_info = out_all[i].split("#:#");
    $('#files').append(
      "<a class=\"data\" data-src=\"DCIM/"+dir+"/"+subdir+"/"+out_info[0]+"\" data-dos=\""+out_info[0]+"\">"
      +" <div class=\"media\" >"
      +"  <div class=\"media-left\">"
      +"   <img src=\"DCIM/"+dir+"/"+subdir+"/"+out_info[0]
               +"\" class=\"media-object align-self-center\" style=\"width:60px\">"
      +"  </div>"
      +"  <div class=\"media-body text-center\">"
      +"   <h4 class=\"media-heading\">"+out_info[0]+"</h4>"
      +"   <p> "+out_info[2] +" , "+out_info[1]+"</p>"
      //+"   <p>"+out_info[3]+"</p>"
      +"  </div>"
      +" </div>"
      +"</a>"
     );
   }
 });
}


function getDirs(dir) {
 $.get("cgi-bin/media.cgi", {cmd: "getDirs", dir: dir}, function(out){
   $('#dirs').html("");
   $('#files').html("");
   var out_all = out.split("\n");
   if ( out_all.length == 1)
    $('#dirs').html("<h1>No files found</h1>");
   var unuo=1;
   for (var i = 0; i < out_all.length-1; i++)
   {
    var out_info = out_all[i].split("#:#");
    if (out_info[1] == "0")
      continue;
    if ( unuo==1 )
    {
     $('#dirs').append( "<li class=\"nav-item\"><a class=\"nav-link active\" href=\"#\" data-subdir=\""+out_info[0]+"\">"
	+ out_info[0]+" <span class=\"badge badge-pill badge-info\">"+out_info[1]+"</span></a></li>");
     getFiles(dir,out_info[0]);
     unuo=0;
    }
    else
     $('#dirs').append( "<li class=\"nav-item\"><a class=\"nav-link\" href=\"#\" data-subdir=\""+out_info[0]+"\">"
	+ out_info[0]+" <span class=\"badge badge-pill badge-info\">"+out_info[1]+"</span></a></li>");
   }
  }
 );
}

