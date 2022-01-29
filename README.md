PostgreSQL implementation of Convert HTML to PDF using Webkit (QtWebKit).

### [Use of the extension](#use-of-the-extension)

```sql
select wkhtmltopdf_set_object_setting('page', 'https://github.com');
copy (
    select wkhtmltopdf_convert()
) to '/var/lib/postgresql/wkhtmltopdf.pdf' WITH (FORMAT binary, HEADER false)
```
