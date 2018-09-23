Feeds Tamper
============

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Examples
 * Troubleshooting
 * Maintainers


INTRODUCTION
------------

Feeds Tamper provides a small plugin architecture for Feeds to modify data
before it gets saved. Several plugins are available by default and are described
in the examples section below. Additional plugins can be added in separate
modules or through the issue queue.

A full list of plugins: https://www.drupal.org/node/1246578
Contributed plugins: https://www.drupal.org/node/2455399

Creating new plugins is easy, see the developer docs:
https://www.drupal.org/node/1246602


REQUIREMENTS
----------

 * Feeds


INSTALLATION
------------

Feeds Tamper can be installed like any other Drupal module.

 * Place it in the modules directory for your site and enable it on the
   `admin/modules` page.
 * Enable the Feeds Tamper Admin UI module to be able to configure tampers in
   the UI.


CONFIGURATION
-------------

Enable the modules Feeds Tamper and Feeds Tamper Admin UI. Next, configure the
plugins you want to use to modify Feeds data before it gets saved. Each Feeds
import mapping can be manipulated individually.

Once Feeds Tamper is enabled, the list of available Feeds importers
(admin/structure/feeds) displays a "Tamper" configuration link for each
importer.


EXAMPLES
--------

 * Replace every instance of 'dog' with 'cat'.
 * Filter items based on keywords or vocabularies.
 * Make every letter uppercase, lowercase, or capitalize every first letter.
 * Break a comma separated list of words into Taxonomy terms or
   a multivalued text field.
 * Combine separate 'firstname' and 'lastname' fields into one 'name' field.
 * Convert urls from relative to absolute.
 * See the full list (https://www.drupal.org/node/1246578).
 * Incredibly simple plugin architecture allowing you to do almost anything
   to Feeds' data. This comes with simple configuration and exportability
   (i.e. Features.)


TROUBLESHOOTING
---------------

Check out the documentation (https://www.drupal.org/node/1246562).


MAINTAINERS
-----------

 * Chris Leppanen (twistor) - https://www.drupal.org/u/twistor
 * Youri van Koppen (MegaChriz) - https://www.drupal.org/u/megachriz
 * Franz Glauber Vanderlinde (franz) - https://www.drupal.org/u/franz
