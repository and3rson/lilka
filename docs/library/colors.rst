``colors``: 16-бітні кольори
============================

.. raw:: html

    <div style="margin: 1rem 0">
        <a id="sort-by-name" href="javascript:void(0);" onclick="sortByName()" style="font-weight: bold">Сортувати за назвами</a>
        |
        <a id="sort-by-hue" href="javascript:void(0);" onclick="sortByHue()">Сортувати за тонами (hue)</a>
    </div>

.. doxygenenum:: lilka::colors::RGB565_Colors

.. raw:: html

    <style type="text/css">
        dl.cpp.enumerator {
            display: flex;
            flex-direction: row;
            align-items: center;
        }
        dl.cpp.enumerator dt {
            margin: 0 !important;
        }
        dl.cpp.enumerator dd {
            margin: 0;
        }
    </style>

    <script type="text/javascript">
        function convertRGB565to888(rgb565) {
            // Extracting red, green, and blue components from RGB 565
            let red = (rgb565 >> 11) & 0x1F;    // 5 bits for red
            let green = (rgb565 >> 5) & 0x3F;   // 6 bits for green
            let blue = rgb565 & 0x1F;            // 5 bits for blue

            // Scaling the components to fit the range of RGB 888
            red = (red * 255) / 31;
            green = (green * 255) / 63;
            blue = (blue * 255) / 31;

            // Combining the components into RGB 888 format
            let rgb888 = (red << 16) | (green << 8) | blue;

            // Converting the result to hexadecimal and adding leading zeros if necessary
            let hexColor = rgb888.toString(16).toUpperCase().padStart(6, '0');

            return "#" + hexColor;
        }

        function getRGB565Components(rgb565) {
            // Extracting red, green, and blue components from RGB 565
            let red = (rgb565 >> 11) & 0x1F;    // 5 bits for red
            let green = (rgb565 >> 5) & 0x3F;   // 6 bits for green
            let blue = rgb565 & 0x1F;            // 5 bits for blue

            return {red, green, blue};
        }

        function getContrastTextColor(hexColor) {
            // Remove '#' if present
            hexColor = hexColor.replace('#', '');

            // Convert hexadecimal color to RGB
            let r = parseInt(hexColor.substring(0,2), 16);
            let g = parseInt(hexColor.substring(2,4), 16);
            let b = parseInt(hexColor.substring(4,6), 16);

            // Calculate luminance
            let luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255;

            // Choose black or white based on luminance
            return (luminance > 0.5) ? '#000000' : '#FFFFFF';
        }

        const dt = document.querySelector('[id*="RGB565_Colors"]');
        const dd = dt.nextElementSibling;
        Array.prototype.forEach.call(dd.querySelectorAll('dl > dd'), function (element) {
            const color565Str = element.innerText.trim();
            const color565 = parseInt(color565Str, 16);
            const color888 = convertRGB565to888(color565);
            const {red, green, blue} = getRGB565Components(color565);
            const textColor = getContrastTextColor(color888);
            element.innerHTML = `<span style="background-color: ${color888}; color: ${textColor}; padding: 0.35rem 2rem; font-family: monospace">${color565Str} (${color888})</span>`;
        });

        // Sort colors

        function sortColors(colors) {
            // Convert hexadecimal colors to HSL
            let hslColors = colors.map(hexToHSL);

            // Sort the colors based on hue
            hslColors.sort((a, b) => a[0] - b[0]);

            // Convert sorted HSL colors back to hexadecimal
            let sortedColors = hslColors.map(hslToHex);

            return sortedColors;
        }

        function hexToHSL(hexColor) {
            // Remove '#' if present
            hexColor = hexColor.replace('#', '');

            // Convert hexadecimal color to RGB
            let r = parseInt(hexColor.substring(0, 2), 16) / 255;
            let g = parseInt(hexColor.substring(2, 4), 16) / 255;
            let b = parseInt(hexColor.substring(4, 6), 16) / 255;

            // Find maximum and minimum values of RGB components
            let max = Math.max(r, g, b);
            let min = Math.min(r, g, b);

            // Calculate hue
            let h;
            if (max === min) {
                h = 0; // Achromatic (grayscale)
            } else {
                let d = max - min;
                switch (max) {
                    case r: h = ((g - b) / d + (g < b ? 6 : 0)) / 6; break;
                    case g: h = ((b - r) / d + 2) / 6; break;
                    case b: h = ((r - g) / d + 4) / 6; break;
                }
            }

            // Calculate lightness
            let l = (max + min) / 2;

            // Calculate saturation
            let s = (l === 1 || l === 0) ? 0 : (max - l) / Math.min(l, 1 - l);

            return [h, s, l];
        }

        function hslToHex(hslColor) {
            let [h, s, l] = hslColor;
            // Convert HSL to RGB
            let r, g, b;
            if (s === 0) {
                r = g = b = l; // Achromatic (grayscale)
            } else {
                let hueToRGB = (p, q, t) => {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (t < 1 / 6) return p + (q - p) * 6 * t;
                    if (t < 1 / 2) return q;
                    if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                    return p;
                };
                let q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                let p = 2 * l - q;
                r = hueToRGB(p, q, h + 1 / 3);
                g = hueToRGB(p, q, h);
                b = hueToRGB(p, q, h - 1 / 3);
            }

            // Convert RGB to hexadecimal
            let toHex = (x) => {
                let hex = Math.round(x * 255).toString(16);
                return hex.length === 1 ? '0' + hex : hex;
            };

            return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
        }

        const sortByName = () => {
            document.getElementById('sort-by-name').style.fontWeight = 'bold';
            document.getElementById('sort-by-hue').style.fontWeight = 'normal';

            const dls = Array.prototype.slice.call(dd.querySelectorAll('dl'));
            dls.sort((a, b) => {
                const titleA = a.querySelector('dt .descname').innerText.trim();
                const titleB = b.querySelector('dt .descname').innerText.trim();
                return titleA.localeCompare(titleB); // Sort by name
            }).forEach(dl => dd.appendChild(dl));
        }

        const sortByHue = () => {
            document.getElementById('sort-by-name').style.fontWeight = 'normal';
            document.getElementById('sort-by-hue').style.fontWeight = 'bold';

            const dls = Array.prototype.slice.call(dd.querySelectorAll('dl'));
            dls.sort((a, b) => {
                const color565StrA = a.querySelector('dd').innerText.trim();
                const color565StrB = b.querySelector('dd').innerText.trim();
                const color565A = parseInt(color565StrA, 16);
                const color565B = parseInt(color565StrB, 16);
                const color888A = convertRGB565to888(color565A);
                const color888B = convertRGB565to888(color565B);
                const colorHSLA = hexToHSL(color888A);
                const colorHSLB = hexToHSL(color888B);
                return colorHSLA[0] - colorHSLB[0]; // Sort by hue
            }).forEach(dl => dd.appendChild(dl));
        }
    </script>
