import csv

def csv_to_markdown(csv_filename):
    with open(csv_filename, newline='', encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        
        # Вибрані стовпці
        selected_columns = ['К-сть', 'Ідентифікатор', 'Значення', 'Посилання']
        
        # Встановлення мінімальних ширин для вибраних стовпців
        col_widths = {
            'К-сть': 6,
            'Ідентифікатор': 16,
            'Значення': 30,
            'Посилання': 110
        }

        # Створення таблиці в Markdown
        markdown = "# V2\n\n## Мінімальний базовий набір\n\n"
        
        # Заголовок таблиці
        markdown += "| " + " | ".join([header.ljust(col_widths[header]) for header in selected_columns]) + " |\n"
        
        # Лінія для вирівнювання
        markdown += "| " + " | ".join(['-' * col_widths[header] for header in selected_columns]) + " |\n"
        
        # Рядки таблиці
        for row in reader:
            markdown += "| " + " | ".join([str(row[header]).ljust(col_widths[header]) if row[header] else '?'.ljust(col_widths[header]) for header in selected_columns]) + " |\n"
        
        return markdown

csv_filename = 'bom.csv'
md_content = csv_to_markdown(csv_filename)

with open('bom.md', 'w', encoding='utf-8') as mdfile:
    mdfile.write(md_content)

print("Markdown table saved to bom.md")
