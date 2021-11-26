
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
    var src="/lib/bt-play.png";
    if ( dir == "pik" )
      src="DCIM/"+dir+"/"+subdir+"/"+out_info[0];
    $('#files').append(
      "<a class=\"data\" data-src=\"DCIM/"+dir+"/"+subdir+"/"+out_info[0]+"\" data-dos=\""+out_info[0]+"\">"
      +" <div class=\"media\" >"
      +"  <div class=\"media-left\">"
      +"   <img decoding=async loading=lazy alt=\"\" src=\""+src
               +"\" class=\"mr-3 align-self-center\" style=\"width:60px\">"
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

/*
function runPictureDialog() {
  var content = $('<div class="picture-dialog-content"></div>');
  var buttons = [
            {caption: "Fermi"},
            {caption: "Elŝuti", isDefault: true, click: function () {
                var entry = entries[pos];
                downloadFile(mediaType + '/' + entry.cameraId + '/download' + entry.path);

                return false;
            }}];

    function bodyKeyDown(e) {
        switch (e.which) {
            case 37:
                if (prevArrow.is(':visible')) {
                    prevArrow.click();
                }
                break;

            case 39:
                if (nextArrow.is(':visible')) {
                    nextArrow.click();
                }
                break;
        }
    }
    $('body').on('keydown', bodyKeyDown);

    runModalDialog({
        title: ' ',
        closeButton: true,
        buttons: buttons,
        content: content,
        stack: true,
        //onShow: updatePicture,
        onClose: function () {
            $('body').off('keydown', bodyKeyDown);
        }
    });
}
*/

/* */
function runPictureDialog(entries, pos, mediaType, onDelete) {
    var content = $('<div class="picture-dialog-content"></div>');

    var img = $('<img class="picture-dialog-content">');
    content.append(img);

    var video_container = $('<video id="mPlayer" class="picture-dialog-content" controls="true">');
    var video_loader = $('<img>');
    video_container.on('error', function(err) {
        var msg = '';

        // Reference: https://html.spec.whatwg.org/multipage/embedded-content.html#error-codes 
        switch (err.target.error.code) {
            case err.target.error.MEDIA_ERR_ABORTED:
                msg = loki18n.gettext('Vi abortis la filmeton.');
                break;
            case err.target.error.MEDIA_ERR_NETWORK:
                msg = loki18n.gettext('Reto eraro okazis.');
                break;
            case err.target.error.MEDIA_ERR_DECODE:
                msg = loki18n.gettext('Malkodado-eraro aŭ neprogresinta funkcio.');
                break;
            case err.target.error.MEDIA_ERR_SRC_NOT_SUPPORTED:
                msg = loki18n.gettext('Formato ne subtenata aŭ neatingebla/netaŭga por ludado.');
                break;
            default:
                msg = loki18n.gettext('Nekonata eraro okazis.');
        }

        showErrorMessage(loki18n.gettext('Eraro : ') + msg);
    });
    video_container.hide();
    content.append(video_container);

    var prevArrow = $('<div class="picture-dialog-prev-arrow button mouse-effect" title="'+loki18n.gettext("antaŭa bildo")+'"></div>');
    content.append(prevArrow);

    var playButtonContainer = $('<div class="picture-dialog-playbuttons"></div>');

      var playButton = $('<div class="picture-dialog-play button mouse-effect" title="'+loki18n.gettext("ludi")+'"></div>');
      playButtonContainer.append(playButton);
    
      var timelapseButton = $('<div class="picture-dialog-timelapse button mouse-effect" title="'+loki18n.gettext("ludi * 5 kaj enĉenigi")+'"></div>');
      playButtonContainer.append(timelapseButton);

    content.append(playButtonContainer);

    var nextArrow = $('<div class="picture-dialog-next-arrow button mouse-effect" title="'+loki18n.gettext("sekva bildo")+'"></div>');
    content.append(nextArrow);
    var progressImg = $('<div class="picture-dialog-progress">');

    function updatePicture() {
        var entry = entries[pos];

        var windowWidth = $(window).width();
        var windowHeight = $(window).height();
        var widthCoef = windowWidth < 1000 ? 0.8 : 0.5;
        var heightCoef = 0.75;

        var width = parseInt(windowWidth * widthCoef);
        var height = parseInt(windowHeight * heightCoef);

        prevArrow.css('display', 'none');
        nextArrow.css('display', 'none');

        var mPlayer = document.getElementById('mPlayer');
        var playable = video_container.get(0).canPlayType(entry.mimeType) != '';
        timelapseButton.hide();
        playButton.hide();
        video_container.hide();
        img.show();

        img.parent().append(progressImg);
        updateModalDialogPosition();

        img.attr('src', addAuthParams('GET', basePath + mediaType + '/' + entry.cameraId + '/preview' + entry.path));

        if (playable) {
            video_loader.attr('src', addAuthParams('GET', basePath + mediaType + '/' + entry.cameraId + '/playback' + entry.path));
            playButton.on('click', function() {
                video_container.attr('src', addAuthParams('GET', basePath + mediaType + '/' + entry.cameraId + '/playback' + entry.path));
                video_container.show();
                video_container.get(0).load();  // Must call load() after changing <video> source 
                img.hide();
                playButton.hide();
                timelapseButton.hide();
                video_container.on('canplay', function() {
                   video_container.get(0).play();  // Automatically play the video once the browser is ready 
                });
            });

            timelapseButton.on('click', function() {
                playButton.click();
                mPlayer.playbackRate = 5;
                video_container.on('ended', function() {
                    if( pos > 0 ) {
                        nextArrow.click();
                        playButton.click();
                        mPlayer.playbackRate = 5;
                    }
                });
            });

            playButton.show();
            timelapseButton.show();
        }

        img.load(function () {
            var aspectRatio = this.naturalWidth / this.naturalHeight;
            var sizeWidth = width * width / aspectRatio;
            var sizeHeight = height * aspectRatio * height;

            img.width('').height('');
            video_container.width('').height('');

            if (sizeWidth < sizeHeight) {
                img.width(width);
                video_container.width(width).height(parseInt(width/aspectRatio));
            }
            else {
                img.height(height);
                video_container.width(parseInt(height*aspectRatio)).height(height);
            }
            updateModalDialogPosition();
            prevArrow.css('display', pos < entries.length - 1 ? '' : 'none');
            nextArrow.css('display', pos > 0 ? '' : 'none');
            progressImg.remove();
        });

        modalContainer.find('span.modal-title:last').html(entry.name);
        updateModalDialogPosition();
    }

    prevArrow.click(function () {
        if (pos < entries.length - 1) {
            pos++;
        }

        updatePicture();
    });

    nextArrow.click(function () {
        if (pos > 0) {
            pos--;
        }

        updatePicture();
    });

    function bodyKeyDown(e) {
        switch (e.which) {
            case 37:
                if (prevArrow.is(':visible')) {
                    prevArrow.click();
                }
                break;

            case 39:
                if (nextArrow.is(':visible')) {
                    nextArrow.click();
                }
                break;
        }
    }

    $('body').on('keydown', bodyKeyDown);

    //img.load(updateModalDialogPosition);

    var buttons = [
            {caption: loki18n.gettext("Fermi")},
            {caption: loki18n.gettext("Elŝuti"), isDefault: true, click: function () {
                var entry = entries[pos];
                downloadFile(mediaType + '/' + entry.cameraId + '/download' + entry.path);

                return false;
            }}];
    //if (isAdmin())
    {
        buttons.push({
                caption: loki18n.gettext("Forigi"),
                isDefault: false,
                className: 'delete',
                click: function () {
                    var entry = entries[pos];
                    var callback = function() {
                        onDelete(entry);
                        if (entries.length > 0) {
                            if (pos > entries.length -1) {
                                pos--;
                            }
                            updatePicture();
                        } else {
                            hideModalDialog(); // Close dialog after deleting the only remaining entry 
                        }
                    }
                    deleteFile(mediaType + '/' + entry.cameraId + '/delete' + entry.path, callback);

                    return false;
                }
            });
    }

    runModalDialog({
        title: ' ',
        closeButton: true,
        buttons: buttons,
        content: content,
        stack: true,
        onShow: updatePicture,
        onClose: function () {
            $('body').off('keydown', bodyKeyDown);
        }
    });
}
/* */
