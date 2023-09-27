function parse_search_string(href) {
    var parser = document.createElement('a');
    var res = {};
    parser.href = href;
    // say parser.search == "?a=1&b=2&c=3";
    if (parser.search != "") {
        var kv_list = parser.search.slice(1).split("&");
        // kv_list = [ "a=1", "b=2", "c=3" ]
        var kv_dic = {};
        for (var i = 0; i < kv_list.length; i++) {
            var kv = kv_list[i];
            var j = kv.indexOf("=");
            var key = kv.slice(0, j);
            var val = kv.slice(j + 1);
            res[key] = val;
        }
    }
    return res;
}

function main() {
    var params = parse_search_string(window.location.href);
    var lang = params["lang"];
    if (lang == null) {
        lang = "ja";
    }
    var lang_to_hide = null;
    if (lang == "en") {
        lang_to_hide = "ja";
    } else {
        lang_to_hide = "en";
    }
    var nodes_to_hide = document.getElementsByClassName(lang_to_hide);
    for (i = 0; i < nodes_to_hide.length; i++) {
        nodes_to_hide[i].style.display = "none";
    }
}

