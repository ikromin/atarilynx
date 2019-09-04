var canvas = $('#canvas');
var dndArea = $('.at_play_dnd_area');
var canvasScale = 2;
var canvasRotation = 0;

var Module = {
  noInitialRun: true,
  print: function(text) { },
  printErr: function(text) { },
  preRun: [],
  postRun: [],
  canvas: canvas[0],
  totalDependencies: 0,
  locateFile: function(path, prefix) {
    if (path == 'handy_libretro.wasm') {
      return '/wasm/handy_libretro.wasm';
    }
    return path;
  },
  onRuntimeInitialized: function() {
    dndArea.text('Atari Lynx Emulator Ready');
  }
};

function setupFileSystem()
{
  dndArea.text('Loading File System...');

  var r = getParameterByName('r');
  suffix = '';
  if (r == '1') {
    suffix = '_90';
    canvasRotation = 90;
  }
  else if (r == '2') {
    suffix = '_270';
    canvasRotation = 270;
  }

  $('#canvas_div').css({
    'transform': 'rotate(' + canvasRotation + 'deg)'
  });

  var fsOpts = {
    fs: "MountableFileSystem",
    options: {
      '/home/web_user/retroarch/userdata/content': {
        fs: 'InMemory'
      },
      '/home/web_user/retroarch/userdata': {
        fs: 'XmlHttpRequest',
        options: {
          baseUrl: '/retroarch' + suffix + '/',
          index: {
            "retroarch.cfg":null
          },
          preferXHR: true
        }
      },
      '/home/web_user/retroarch/userdata/system': {
        fs: 'XmlHttpRequest',
        options: {
          baseUrl: '/retroarch_system/',
          index: {
            "lynxboot.img":null
          },
          preferXHR: true
        }
      }
    }
  };

  BrowserFS.configure(fsOpts, function(error) {
    if (error) {
      dndArea.text('ERROR: ' + error);
      return;
    }
    
    var BFS = new BrowserFS.EmscriptenFS();
    FS.mount(BFS, {root: '/home'}, '/home');
  });
}

function uploadData(data, name)
{
   var dataView = new Uint8Array(data);
   var fileName = '/home/web_user/retroarch/userdata/content/' + name;

   FS.writeFile(fileName, dataView, { encoding: 'binary' });

   Module['callMain']([fileName]);
   dndArea.text('Running');
   
   $('canvas')[0].exitPointerLock();
   fixCanvasSize();
   
   $('#at_rom_select').hide();
}

function fixCanvasSize() {
  var w = 160;
  var h = 102;

  w *= canvasScale;
  h *= canvasScale;

  if (canvas.width() != w) {
    canvas.css({
      'width': w + 'px',
      'height': h + 'px'
    });
    canvas.attr(
      {
        'width': w,
        'height': h
      }
    );
  }
  setTimeout(fixCanvasSize, 2000);
}

function uploadFileFunc(droppedFiles) {
  var file = droppedFiles[0];
  var fname = file.name.toLowerCase();
  
  // make sure that a Lynx ROM file was dropped
  if (fname.endsWith('.lnx') || fname.endsWith('.o') || fname.endsWith('.lyx')) {
    dndArea.text('Loading ROM...');

    var filereader = new FileReader();
    filereader.file_name = file.name;
    filereader.readAsArrayBuffer(file);
    
    filereader.onload = function() {
      uploadData(this.result, this.file_name);
    };
    filereader.onerror = function() {
      dndArea.text('Load ROM failed!')
    }
  }
  else {
  	dndArea.text('Please select a LNX ROM file!');
  }
}

$(document).ready(function() {
  dndArea.on('drag dragstart dragend dragover dragenter dragleave drop', function(event) {
    event.preventDefault();
    event.stopPropagation();

    if (event.type == 'dragenter') {
      $(event.currentTarget).addClass('at_play_dnd_area_hover');
    }
    else if (event.type == 'dragleave' || event.type == 'dragend' || event.type == 'drop') {
      $(event.currentTarget).removeClass('at_play_dnd_area_hover');
    }
    
    if (event.type == 'drop') {
      var droppedFiles = event.originalEvent.dataTransfer.files;
      uploadFileFunc(droppedFiles);
    }
  });

  $('#at_file_select').change(function(event) {
    uploadFileFunc(event.target.files);
  });

  $(window).focusout(function() {
    if (Browser.mainLoop.currentlyRunningMainloop) {
      Module.pauseMainLoop();
      dndArea.text('Paused');
    }
  });
  $(window).focusin(function() {
    if (Browser.mainLoop.scheduler == null && Browser.mainLoop.currentlyRunningMainloop) {
      Module.resumeMainLoop();
      dndArea.text('Running');
    }
  });

  var keys = {
    9: "tab",
    13: "enter",
    16: "shift",
    18: "alt",
    27: "esc",
    33: "rePag",
    34: "avPag",
    35: "end",
    36: "home",
    37: "left",
    38: "up",
    39: "right",
    40: "down",
    112: "F1",
    113: "F2",
    114: "F3",
    115: "F4",
    116: "F5",
    117: "F6",
    118: "F7",
    119: "F8",
    120: "F9",
    121: "F10",
    122: "F11",
    123: "F12"
  };
  window.addEventListener('keydown', function (e) {
    if (keys[e.which]) {
      e.preventDefault();
    }
  });

  $('.at_canv_resize').click(function(e) {
    e.preventDefault();
    canvasScale = parseFloat($(e.target).data('value'));
    fixCanvasSize();
  });
   
  setupFileSystem();
});

getParameterByName = function(name)
{
  name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
  var regexS = "[\\?&]" + name + "=([^&#]*)";
  var regex = new RegExp(regexS);
  var results = regex.exec(window.location.search);
  if(results == null)
    return '0';
  else
    return decodeURIComponent(results[1].replace(/\+/g, " "));
}