# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "Лілка"
copyright = "2024, Andrew Dunai"
author = "Andrew Dunai"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = []

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", "_old"]

language = "uk_UA"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_theme_options = {
    "collapse_navigation": False,
    "logo_only": True,
    "titles_only": True,
    "display_version": False,
}
html_logo = "_static/lilka_logo.png"

# -- Custom CSS --------------------------------------------------------------


def setup(app):
    app.add_css_file("extra_style.css")


# -- Breathe configuration ---------------------------------------------------

extensions.append("breathe")

breathe_projects = {
    "lilka": "./_doxygen/xml",
}
breathe_default_project = "lilka"

primary_domain = "cpp"

highlight_language = "cpp"

# -- Lua ---------------------------------------------------------------------

extensions.append("sphinxcontrib.luadomain")
extensions.append("sphinx_lua")

lua_source_path = ["../sdk/addons/lualilka/"]
lua_source_encoding = 'utf8'
lua_source_comment_prefix = '---'
lua_source_use_emmy_lua_syntax = True
lua_source_private_prefix = '_'
