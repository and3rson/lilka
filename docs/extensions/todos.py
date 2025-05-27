import os

from docutils import nodes
from docutils.parsers.rst import Directive


class ToDoExtractor(Directive):
    has_content = True
    option_spec = {
        "include-ext": lambda x: x.lower().split(","),
        "exclude-dirs": lambda x: x.split(","),
    }

    def run(self):
        # Get directory from which we want to extract todos
        directory = self.content[0]

        # Search all .c/.cpp/.h files in the directory recursively

        root_node = nodes.bullet_list()

        for root, dirs, files in os.walk(directory):
            if self.options.get("exclude-dirs"):
                if any(
                    exclude_dir in root for exclude_dir in self.options["exclude-dirs"]
                ):
                    continue
            for file in files:
                full_path = os.path.join(root, file)
                clean_path = full_path.removeprefix("../")
                if self.options.get("include-ext") and not file.lower().endswith(
                    tuple(self.options["include-ext"])
                ):
                    continue
                with open(os.path.join(root, file)) as f:
                    lines = f.readlines()
                    for lineno, line in enumerate(lines, 1):
                        if "TODO" in line:
                            # Create a list item
                            list_node = nodes.list_item()
                            # Add link to "https://github.com/lilka-dev/lilka/blob/main/{clean_path}"
                            filename_para = nodes.paragraph()
                            filename_para += nodes.reference(
                                rawsource=f"{clean_path}:{lineno}",
                                text=f"{clean_path}:{lineno}",
                                refuri=f"https://github.com/lilka-dev/lilka/blob/main/{clean_path}#L{lineno}",
                            )
                            list_node += filename_para
                            # Add content as code block with C/C++ syntax highlighting
                            content_para = nodes.paragraph()
                            # Get this line and previous/next lines
                            text = ''.join(lines[lineno - 2 : lineno + 1])
                            code = nodes.literal_block(text, text)
                            code["language"] = "cpp"
                            content_para += code
                            list_node += content_para
                            root_node += list_node

        return [root_node]


def setup(app):
    app.add_directive("todos", ToDoExtractor)

    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
