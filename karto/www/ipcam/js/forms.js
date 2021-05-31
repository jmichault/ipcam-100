      function setformval(elt,index,array){
        var $el = $('[name="'+elt.name+'"]'),
         type = $el.attr('type');
        $el.val(elt.value);
        switch(type){
          case 'checkbox':
            $el.attr('checked', 'checked');
            break;
          case 'radio':
            $el.filter('[value="'+elt.value+'"]').attr('checked', 'checked');
            break;
          default:
            $el.val(elt.value);
        }
       };

