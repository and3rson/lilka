``colors``: 16-бітні кольори
============================

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
    </script>
