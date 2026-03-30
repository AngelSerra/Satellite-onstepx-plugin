// plugins/satellite/SatDefs.h
#pragma once

// ==================== TEXTOS ====================
#define L_OBSERVER_LOCATION "Observador"
#define L_CONTROLS "Control"
#define L_LATITUDE "Latitud"
#define L_LONGITUDE "Longitud"
#define L_CE_NO_PARK_POSITION_SET "Sin posición de estacionamiento"

// ==================== COLORES CSS ====================
#define COLOR_BORDER "#444444"
#define COLOR_FOREGROUND "#dddddd"
#define COLOR_BACKGROUND "#222222"
#define COLOR_LIGHT_BACKGROUND "#2a2a2a"
#define COLOR_CONTROL "#88ccff"
#define COLOR_CONTROL_BACKGROUND "#1a3a5a"
#define COLOR_COLLAPSIBLE "#88ccff"
#define COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "#1a3a5a"
#define COLOR_COLLAPSIBLE_CONTENT_BACKGROUND "#2a2a2a"
#define COLOR_ALERT "#ff8866"

// ==================== HTML TEMPLATES ====================
const char html_head_begin[] = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
const char html_head_end[] = "</head>";
const char html_body_begin[] = "<body>";
const char html_page_and_body_end[] = "</div></body></html>";
const char html_bad_comms_message[] = "<div class='error'>No communication with OnStep</div>";
const char html_onstep_page_begin[] = "<div class='onstep-page'>";
const char html_tile_end[] = "</div></div>";
const char html_tile_text_beg[] = "<div class='tile' style='width:%s; min-height:%s;'><div class='tile-title'><div class='tile-title-text'>%s</div></div><div class='tile-content'>";

const char html_main_css_begin[] = "<style>";
const char html_main_css_core[] = 
    "body{background:#1a1a1a;color:#ddd;font-family:Arial;margin:0;padding:0;}"
    ".onstep-page{max-width:1200px;margin:0 auto;padding:10px;}";
const char html_main_css_control[] = 
    ".tile{background:#222;border:1px solid #444;margin:10px;padding:0;border-radius:5px;float:left;overflow:hidden;}"
    ".tile-title{background:#1a1a2e;padding:8px;border-bottom:1px solid #444;font-weight:bold;}"
    ".tile-title-text{color:#88ccff;}"
    ".tile-content{padding:10px;}";
const char html_main_css_buttons[] = 
    ".c{color:#88ccff;font-weight:bold;}"
    ".clear{clear:both;}"
    ".error{color:#ff6666;text-align:center;padding:20px;}";
const char html_main_css_collapse[] = "";
const char html_main_css_end[] = "</style>";

const char html_script_ajax[] = "<script>function updateData(){fetch(ajaxPage).then(r=>r.text()).then(t=>{let l=t.split('\\n');for(let i of l){let p=i.indexOf('|');if(p>0){let id=i.substring(0,p).trim();let v=i.substring(p+1).trim();let e=document.getElementById(id);if(e)e.innerHTML=v;}}});setTimeout(updateData,1000);}updateData();</script>";
const char html_script_ajax_get[] = "<script>function getData(){fetch('%s').then(r=>r.text());}setInterval(getData,5000);</script>";
const char html_script_collapsible[] = "<script>var coll=document.getElementsByClassName(\"collapsible\");for(var i=0;i<coll.length;i++){coll[i].addEventListener(\"click\",function(){this.classList.toggle(\"active\");var content=this.nextElementSibling;if(content.style.display===\"block\"){content.style.display=\"none\";}else{content.style.display=\"block\";}});}</script>";