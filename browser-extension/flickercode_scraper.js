(function() {
    'use strict';

    var flickercode_scraper = function() {
        var canvas = document.getElementById('rsct_opttan_canvas')
        var nibbleValue = 0
        var flickerString = ""

        if(canvas) {
            var ctx = canvas.getContext('2d')
            var height = ctx.canvas.height;
            var width = ctx.canvas.width;
            const border_width = 0.1

            var reorderBytes = function(input) {
                let output = ""
                for (var i = 0; i < input.length; i = i + 2) {
                    output += input.substr(i + 1, 1) + input.substr(i, 1);
                }
                return output
            }

            var storeValue = function() {
                flickerString += (nibbleValue >>> 0).toString(16).toUpperCase()

                // remove everything before the "0FF" sync signal
                if(!flickerString.startsWith("0FF")) {
                    let startpos = flickerString.indexOf("0FF")
                    if(startpos > 0) {
                        flickerString = flickerString.substr(startpos)
                    }
                } else {
                    // check if we have another sync signal, if so we're done
                    let endpos = flickerString.indexOf("0FF", 3)
                    if(endpos > 0) {
                        // reset canvas context methods to normal
                        ctx.fillRect = ctx.real_fillRect
                        ctx.clearRect = ctx.real_clearRect

                        // reorder the swapped nibbles and fire event
                        let realString = reorderBytes(flickerString.substr(3, endpos-3))
                        document.dispatchEvent(new CustomEvent('flickercode_scraped', { detail: realString}));
                    }
                }
            }

            // find the bit to set/unset based on x,y passed to ctx.fillRect()/ctx.clearRect()
            var setRect = function(x,y,fill) {
                let num=0
                if(x > 0) {
                    let lwidth = width / 5
                    let border = lwidth * border_width;
                    num = (x - border) / lwidth
                } else {
                    let lheight = height / 5
                    let border = lheight * border_width;
                    num = 4 - (y - border) / lheight;
                }
                if(num == 0 && fill) {
                    storeValue()
                } else {
                    let bit_to_set = 1 << (num-1)
                    if(fill) {
                        nibbleValue |= bit_to_set
                    } else {
                        nibbleValue &= (bit_to_set ^ 0xffff)
                    }
                }
            }

            ctx.real_fillRect = ctx.fillRect
            ctx.fillRect = function(x,y,w,h) {
                ctx.real_fillRect(x,y,w,h)
                setRect(x,y,true)
            }
            ctx.real_clearRect = ctx.clearRect
            ctx.clearRect = function(x,y,w,h) {
                ctx.real_clearRect(x,y,w,h)
                setRect(x,y,false)
            }
        }
    }

    flickercode_scraper()

})();
