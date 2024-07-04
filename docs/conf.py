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
html_favicon = "_static/favicon.ico"
# html_additional_pages = {
#     'GitHub': 'https://github.com/and3rson/lilka',
# }

# https://protips.readthedocs.io/pdf-font.html
latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    "papersize": "a4paper",
    # The font size ('10pt', '11pt' or '12pt').
    "pointsize": "11pt",
    # Additional stuff for the LaTeX preamble.
    "preamble": r"""
        \usepackage{charter}
        \usepackage[defaultsans]{lato}
        \usepackage{inconsolata}
    """,
}

# -- Custom CSS/JS -----------------------------------------------------------


def setup(app):
    app.add_css_file("extra_style.css")
    app.add_js_file("custom.js")


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
lua_source_encoding = "utf8"
lua_source_comment_prefix = "---"
lua_source_use_emmy_lua_syntax = True
lua_source_private_prefix = "_"

# -- Toolbox -----------------------------------------------------------------

extensions.append("sphinx_toolbox.sidebar_links")
extensions.append("sphinx_toolbox.github")
github_username = "and3rson"
github_repository = "lilka"

# -- Graphviz ----------------------------------------------------------------

extensions.append("sphinx.ext.graphviz")

# -- Sphinx-Design ------------------------------------------------------------
extensions.append("sphinx_design")

# -- Hoverxref ---------------------------------------------------------------
extensions.append("hoverxref.extension")
hoverxref_roles = ["term"]
hoverxref_role_types = {
    "term": "tooltip",
}

# -- Images ------------------------------------------------------------------
extensions.append("sphinxcontrib.images")
